# Visão Zambeziana Inteligente (VZI) — versão web com núcleo C++

Este é um ponto de partida para publicar o VZI na Internet:
- interface web responsiva em português;
- servidor/backend em C++;
- endpoint `/api/chat`;
- integração opcional com a API da OpenAI;
- a chave da API fica no servidor, numa variável de ambiente, nunca no navegador.

## Requisitos
- CMake 3.20+
- compilador C++17 (GCC/Clang/MSVC)
- conexão à Internet na primeira compilação para descarregar as dependências
- uma chave de API do fornecedor de IA, se quiser ativar respostas generativas

Dependências descarregadas na compilação:
- cpp-httplib (servidor HTTP)
- nlohmann/json (JSON)

## Compilar (Linux)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/vzi_server
```

Por padrão, o servidor abre em `http://localhost:8080`.

## Windows
Instale CMake e um compilador C++ (por exemplo, Visual Studio com Desktop development with C++). Depois:
```powershell
cmake -S . -B build
cmake --build build --config Release
```
Execute o binário produzido. A localização depende do gerador do CMake.

## Ativar respostas de IA
Defina `OPENAI_API_KEY` no ambiente antes de iniciar o servidor. Opcionalmente, defina `OPENAI_MODEL` (padrão: `gpt-4.1-mini`).

Linux/macOS:
```bash
export OPENAI_API_KEY="a_sua_chave"
export OPENAI_MODEL="gpt-4.1-mini"
./build/vzi_server
```

PowerShell:
```powershell
$env:OPENAI_API_KEY="a_sua_chave"
$env:OPENAI_MODEL="gpt-4.1-mini"
.uildzi_server.exe
```

A chave é usada apenas pelo backend. Não a coloque em `public/index.html`, no Git ou em mensagens públicas. A utilização da API pode ter custos.

## Teste sem chave
Sem `OPENAI_API_KEY`, o servidor devolve uma resposta de demonstração e deixa claro que a IA real ainda não está ativada.

## Publicar na Internet
Este projeto ainda não está publicado. Para o disponibilizar:
1. escolha um servidor/cloud que aceite C++ e CMake;
2. configure `OPENAI_API_KEY` como segredo/variável de ambiente;
3. configure HTTPS e um domínio;
4. inicie o servidor através de um serviço de processos (systemd, container ou equivalente);
5. limite pedidos, registe erros sem guardar conversas sensíveis e defina política de privacidade.

O servidor de demonstração não inclui autenticação, rate limiting, base de dados, moderação avançada nem gestão de consentimento juridicamente validada. Antes de recolher dados reais, implemente esses controlos. Não solicite nomes, contactos ou detalhes íntimos desnecessários.
