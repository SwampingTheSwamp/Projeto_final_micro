# Casa Auto — Sistema de Automação Residencial

## 1. Descrição do projeto

Desenvolvimento de um sistema de automação residencial utilizando o microcontrolador **PIC18F4550**, com interface LCD 16x2, três botões de navegação e display de sete segmentos.

O sistema permitirá controlar iluminação, monitorar temperatura, controlar ventilação e aquecimento, configurar timers e manter um relógio para executar tarefas programadas.

## 2. Funcionalidades planejadas

### Iluminação
- Controle de 8 LEDs representando as luzes da residência.
- Acender ou apagar todas as luzes.
- Acender ou apagar metade das luzes.
- Controle individual por cômodo: sala, cozinha, entrada, sala de estar e banheiro.
- Agendamento para acender ou apagar determinado cômodo em horários específicos.

### Temperatura
- Leitura da temperatura utilizando sensor LM35.
- Exibição da temperatura atual no LCD.
- Controle manual da velocidade da ventoinha.
- Controle automático da ventilação conforme a temperatura.
- Utilização de uma lâmpada para aquecimento.
- Configuração dos limites de temperatura para ventilação e aquecimento.

### Timer
- Criação de timers personalizados.
- Definição de nome e duração.
- Funcionamento em segundo plano.
- Exibição de alerta no LCD quando o tempo terminar.

### Opções e relógio
- Configuração do horário atual.
- Exibição contínua do horário no display de sete segmentos.
- Utilização preferencial de um módulo RTC DS3231 para manter a precisão e preservar o horário quando o sistema estiver desligado.

## 3. Interface do usuário

A navegação será realizada por três botões:

- **RB0:** próxima opção.
- **RB1:** opção anterior.
- **RB2:** confirmar / entrar.

O LCD 16x2 será responsável pela apresentação dos menus e informações.

Estrutura prevista:

```text
MENU CASA
Bem vindo!
```

Menu principal:

```text
> Iluminacao
  Temperatura
```

```text
  Temperatura
> Timer
```

```text
> Timer
  Opcoes
```

Cada opção terá seus respectivos submenus.

## 4. Arquitetura do software

O programa será organizado em módulos independentes:

- **Interface:** navegação, menus e atualização do LCD.
- **Iluminação:** controle dos LEDs e agendamentos.
- **Temperatura:** leitura do sensor, ventilação e aquecimento.
- **Timer:** gerenciamento de contagens e alertas.
- **Relógio:** controle de horário e comunicação com RTC.
- **Temporização:** utilização do Timer0 e interrupções para tarefas periódicas.

A interface não deverá bloquear a execução das demais funcionalidades. Os timers, agendamentos e controles automáticos deverão continuar funcionando independentemente do menu aberto.

## 5. Roteiro de desenvolvimento

- [ ] **Etapa 1 — Estrutura básica**
  - Configuração do PIC18F4550.
  - Inicialização do LCD.
  - Leitura dos três botões.
  - Implementação da máquina de estados.
  - Criação do menu inicial, principal e submenus.

- [ ] **Etapa 2 — Temporização**
  - Configuração do Timer0.
  - Implementação da rotina de interrupção.
  - Criação de uma base de tempo para o sistema.

- [ ] **Etapa 3 — Iluminação**
  - Controle dos oito LEDs.
  - Controle geral e por cômodo.
  - Implementação dos agendamentos.

- [ ] **Etapa 4 — Temperatura**
  - Leitura do LM35.
  - Controle manual da ventoinha.
  - Implementação do PWM.
  - Controle automático de ventilação e aquecimento.

- [ ] **Etapa 5 — Relógio**
  - Integração do RTC DS3231.
  - Configuração do horário.
  - Exibição no display de sete segmentos.

- [ ] **Etapa 6 — Timers**
  - Criação e configuração de timers.
  - Gerenciamento em segundo plano.
  - Alertas de conclusão no LCD.

- [ ] **Etapa 7 — Integração e testes**
  - Integração de todos os módulos.
  - Testes de funcionamento simultâneo.
  - Verificação de conflitos entre interrupções e periféricos.
  - Correção de erros e otimização do código.

## 6. Objetivo final

Construir um sistema embarcado de automação residencial capaz de gerenciar diferentes funcionalidades simultaneamente, utilizando uma interface simples, modular e expansível, com foco no aprendizado de microcontroladores, interrupções, temporização, conversores, PWM e comunicação entre periféricos.
