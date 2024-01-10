# Sistema Remoto de Segurança para Veículos (SRSV) utilizando Reconhecimento Facial

## Visão Geral
O SRSV é um sistema de segurança veicular, que utiliza tecnologia de reconhecimento facial e rastreamento para aumentar a proteção dos veículos. O projeto foi desenvolvido para o curso de Sistemas Eletrônicos Embarcados, componente da graduação em Engenharia Eletrônica na Universidade de Brasília.

## Características
- **Reconhecimento Facial**: Identificação do condutor via câmera.
- **Sensor de Presença**: Detecção de movimento no interior do veículo.
- **Rastreamento GPS**: Localização do veículo em tempo real.
- **Comunicação Remota**: Alertas via SMS ao proprietário.

## Tecnologias Utilizadas
- Raspberry Pi 3B+
- Câmera USB
- Sensor de presença PIR-HC-SR501
- Módulo GPS VK16E
- Módulo GSM SIM800L

## Funcionamento

O Sistema Remoto de Segurança para Veículos (SRSV) possui um funcionamento integrado e sofisticado. Inicia com a detecção de movimento dentro do veículo por um sensor de presença PIR-HC-SR501. Uma vez ativado, uma câmera acoplada ao sistema inicia o processo de reconhecimento facial. Caso o condutor identificado não seja o proprietário registrado, o módulo GSM do sistema envia um SMS com a localização GPS atual do veículo para o celular do proprietário. O coração do sistema é uma Raspberry Pi, responsável pelo processamento de imagens e controle central. A detecção e comunicação são rápidas e eficazes, permitindo uma resposta imediata em casos de invasão ou roubo do veículo.

## Instalação
Veja a documentação para detalhes de instalação e configuração. Em especial o .txt contém link de instruções para configurações chaves do projeto.

## Licença
Distribuído sob a licença MIT. Veja o arquivo LICENSE para mais detalhes.

