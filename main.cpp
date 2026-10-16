#include <cstdlib>
#include <iostream>
#include <string>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static std::string envOr(const char* key, const std::string& fallback) {
    const char* value = std::getenv(key);
    return (value && *value) ? value : fallback;
}

static const std::string SYSTEM_PROMPT =
    "És o assistente da Visão Zambeziana Inteligente (VZI), uma iniciativa "
    "da Associação Visão Zambeziana. Conversa em português claro e respeitoso "
    "com pessoas de Moçambique. Podes abordar objetivos de vida, gostos, "
    "relacionamentos saudáveis, educação, emprego, empreendedorismo, sociedade, "
    "ética e perspetivas para o futuro do país. Faz perguntas de seguimento úteis, "
    "sem pressionar o utilizador. Em temas íntimos, respeita limites e não peças "
    "dados identificáveis. Não assumes que uma resposta representa todos os "
    "moçambicanos. Distingue factos de opiniões e reconhece incerteza. Não "
    "apresentes aconselhamento médico, jurídico ou financeiro como substituto "
    "de um profissional. Responde de forma construtiva e imparcial.";

int main() {
    const std::string apiKey = envOr("OPENAI_API_KEY", "");
    const std::string model = envOr("OPENAI_MODEL", "gpt-4.1-mini");
    const std::string portText = envOr("PORT", "8080");
    int port = 8080;
    try { port = std::stoi(portText); } catch (...) {}

    httplib::Server server;
    server.set_payload_max_length(64 * 1024);

    server.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"({"status":"ok","app":"VZI"})", "application/json; charset=utf-8");
    });

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_file_content("public/index.html", "text/html; charset=utf-8");
    });

    server.Post("/api/chat", [apiKey, model](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Cache-Control", "no-store");

        json body;
        try {
            body = json::parse(req.body);
        } catch (...) {
            res.status = 400;
            res.set_content(R"({"error":"Pedido JSON inválido."})", "application/json; charset=utf-8");
            return;
        }

        if (!body.contains("message") || !body["message"].is_string()) {
            res.status = 400;
            res.set_content(R"({"error":"Inclua uma mensagem de texto."})", "application/json; charset=utf-8");
            return;
        }

        std::string message = body["message"].get<std::string>();
        if (message.size() > 8000) {
            res.status = 413;
            res.set_content(R"({"error":"A mensagem é demasiado longa."})", "application/json; charset=utf-8");
            return;
        }

        if (apiKey.empty()) {
            json out = {
                {"reply", "Estou em modo de demonstração. Para ativar respostas de inteligência artificial, o administrador deve configurar OPENAI_API_KEY no servidor. Entretanto, posso ajudar a explorar objetivos de vida, educação, emprego, relacionamentos saudáveis e o futuro de Moçambique."},
                {"mode", "demo"}
            };
            res.set_content(out.dump(2), "application/json; charset=utf-8");
            return;
        }

        // A chamada de rede deve ser feita no servidor; a chave nunca é enviada ao browser.
        httplib::SSLClient client("api.openai.com", 443);
        client.set_connection_timeout(5, 0);
        client.set_read_timeout(45, 0);
        client.set_write_timeout(10, 0);
        client.set_follow_location(false);

        json payload = {
            {"model", model},
            {"instructions", SYSTEM_PROMPT},
            {"input", message},
            {"max_output_tokens", 700}
        };

        httplib::Headers headers = {
            {"Authorization", "Bearer " + apiKey},
            {"Content-Type", "application/json"}
        };

        auto result = client.Post("/v1/responses", headers, payload.dump(), "application/json");
        if (!result) {
            res.status = 502;
            res.set_content(R"({"error":"Não foi possível contactar o serviço de IA. Tente novamente mais tarde."})",
                            "application/json; charset=utf-8");
            return;
        }
        if (result->status < 200 || result->status >= 300) {
            std::cerr << "Fornecedor de IA devolveu HTTP " << result->status << "\n";
            res.status = 502;
            res.set_content(R"({"error":"O serviço de IA não aceitou o pedido. Verifique a configuração da API."})",
                            "application/json; charset=utf-8");
            return;
        }

        try {
            json answer = json::parse(result->body);
            std::string reply;
            if (answer.contains("output") && answer["output"].is_array()) {
                for (const auto& item : answer["output"]) {
                    if (!item.contains("content") || !item["content"].is_array()) continue;
                    for (const auto& content : item["content"]) {
                        if (content.value("type", "") == "output_text" && content.contains("text")) {
                            if (!reply.empty()) reply += "\n";
                            reply += content["text"].get<std::string>();
                        }
                    }
                }
            }
            if (reply.empty()) reply = "Não consegui produzir uma resposta. Reformula a pergunta, por favor.";
            res.set_content(json({{"reply", reply}, {"mode", "ai"}}).dump(2),
                            "application/json; charset=utf-8");
        } catch (...) {
            res.status = 502;
            res.set_content(R"({"error":"A resposta do serviço de IA veio num formato inesperado."})",
                            "application/json; charset=utf-8");
        }
    });

    std::cout << "VZI a funcionar em http://localhost:" << port << "\n";
    std::cout << "Para respostas de IA, configure OPENAI_API_KEY no ambiente.\n";
    if (!server.listen("0.0.0.0", port)) {
        std::cerr << "Não foi possível iniciar o servidor na porta " << port << ".\n";
        return 1;
    }
    return 0;
}
