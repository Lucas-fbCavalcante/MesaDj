# Mesa de DJ - Simulação com Threads

## Objetivo

Este projeto simula uma mesa de DJ onde diferentes faixas musicais (instrumentos) tocam simultaneamente, cada uma em sua própria thread, de forma totalmente independente. O usuário (DJ) pode interagir com o sistema através de comandos de texto para pausar e retomar a reprodução de cada faixa individualmente, sem afetar as demais.

O trabalho explora conceitos de programação concorrente em C++:
- Criação e controle de threads (`std::thread`)
- Sincronização de acesso a dados compartilhados (`std::mutex`)
- Comunicação entre a thread principal (DJ) e as threads dos instrumentos

## Como compilar

```bash
g++ mesaDj.cpp -o mesaDj.exe -lwinmm
```

## Como executar

```bash
.\mesaDj.exe
```

## Comandos disponíveis

| Comando | Efeito |
|---|---|
| `pausar <nome>` | Pausa o instrumento indicado |
| `tocar <nome>` | Retoma o instrumento indicado |
| `capela` | Muta todos os instrumentos, deixando só o vocal tocando |
| `sair` | Encerra o programa |

Instrumentos disponíveis: `vocal`, `bateria`, `piano`, `guitarra`

Exemplo:
```
pausar bateria
tocar bateria
capela
sair
```

## Equipe

- Breno Luiz de Lima Cruz
- Davi Magno Campelo do Nascimento
- Lauan Gonçalves dos Santos
- Letícia Gomes da Silva
- Lucas Felipe Barreto Cavalcante
- Lucas Filipe de Lima Segundo
- Mateus Valerino Barros de Santana