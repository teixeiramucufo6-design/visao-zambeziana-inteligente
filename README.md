# Visão Zambeziana Inteligente (VZI)

Projeto web com interface em português e backend C++17.

## Estrutura correta
- `CMakeLists.txt`
- `Dockerfile`
- `src/main.cpp`
- `public/index.html`

## Publicar no Render
1. Envie os ficheiros/pastas deste projeto para a raiz do repositório GitHub, mantendo exatamente a estrutura acima.
2. No Render, crie um novo serviço do tipo **Web Service** e ligue o repositório GitHub.
3. Escolha **Docker** como ambiente de execução (o Render deteta o `Dockerfile`).
4. Depois do deploy, o serviço usa a variável `PORT` fornecida pelo Render.
5. Para respostas de IA reais, adicione `OPENAI_API_KEY` em Environment no Render. Opcional: `OPENAI_MODEL=gpt-4.1-mini`.

Sem `OPENAI_API_KEY`, a aplicação responde em modo de demonstração.

**Segurança:** nunca coloque a chave da API no código, no `index.html`, nem no GitHub. A API pode ter custos. Antes de recolher dados pessoais de utilizadores, acrescente limites de pedidos, política de privacidade e controlos de segurança.
