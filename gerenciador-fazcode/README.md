# Gerenciador Fazcode

**Process Monitor & Auto-Restart** — Ferramenta de monitoramento de processos com reinício automático em caso de crash.

Inspirado no RestartOnCrash, com interface moderna **Dark/Pink Neon**.

## Funcionalidades

- **Monitoramento de Processos** — Monitora processos em tempo real
- **Auto-Restart** — Reinicia automaticamente processos que crasham ou param
- **Busca de Processos do Sistema** — Browse processos rodando no sistema
- **Log de Atividades** — Registro completo de eventos, restarts e erros
- **Configurações** — Intervalo de verificação, max tentativas, cooldown
- **Interface Neon** — Tema dark com acentos pink neon

## Instalação

```bash
cd gerenciador-fazcode
pip install -r requirements.txt
python app.py
```

Acesse: `http://localhost:5000`

## Uso

1. Clique em **"+ Adicionar"** para adicionar um processo
2. Configure o nome, comando de inicialização e diretório
3. Clique em **"Iniciar"** para começar o monitoramento
4. O sistema detecta crashes e reinicia automaticamente

## Configurações

| Parâmetro | Padrão | Descrição |
|-----------|--------|-----------|
| Intervalo de Verificação | 5s | Tempo entre verificações |
| Máx. Tentativas | 10 | Máximo de reinícios consecutivos |
| Cooldown | 3s | Espera entre tentativas de reinício |
| Máx. Linhas de Log | 500 | Limite de linhas no log |

## Stack

- **Backend**: Python + Flask + psutil
- **Frontend**: HTML/CSS/JS puro (sem frameworks)
- **Tema**: Dark/Pink Neon com fontes Orbitron + Rajdhani
