# Mojave

Emulador multi-sistema ciclo-a-ciclo para computadores clássicos 8-bit.

## Destaques

- Emulação de CPUs Z80, MOS 6502, MOS 6507 com opcodes documentados e não documentados
- Barramento composto com espaços de endereçamento separados para memória e portas de I/O
- Memória física com regiões ROM somente-leitura e segurança contra limites
- Sistema de tick de dispositivos para temporização ciclo-a-ciclo de periféricos
- TTY virtual para I/O convidado-hospedeiro via mapeamento de porta ou memória
- Telas virtuais plugáveis: qualquer máquina com display escreve em um framebuffer compartilhado

## Pré-requisitos

- **C++20** compilador (g++ 12+ or clang 16+)
- **make** — sistema de build

## Licença

Este projeto está licenciado sob a GNU General Public License v3.0 — veja [LICENSE](LICENSE) para detalhes.
