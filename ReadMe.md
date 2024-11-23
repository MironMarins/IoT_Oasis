Antes de começarmos com o tutorial clone esse repositó no diretorio de sua preferencia.

Para começar a utilizar nossa solução crie um cadastro no site wokwi.com e crie um novo projeto

Copie as informações dos arquivos ("sketch.ino","diagram.jso","libraries.txt") e as cole em suas

respectivas abas localizadas no canto superior esquerdo na tela inicial do novo projeto.

Nosso as informções de nosso dispositivo serão disponibilizadas pelo protocolo MQTT atravez do Node-red.

Preencha as informações relativas ao protocolo MQTT (da linha 56 até a linha 63) de acordo 

com as espcificações pessoais de sua conta no node-red.

Em seguida use importe o arquivo "flows.json" localizado nesse repositório para seu node-red,

clicando na "aba sanduiche" localizada no canto direito da tela do node-red, clique em "importar",

clique em "selecione um arquivo para importar" e procure o arquivo "flows.json" no diretorio onde esse

repositório tenha sido clonado.

Volte para a pagina do dispositivo no wokwi e clique no icone para começar a simulação.

-----------------------------------------------------------------------------------------------
OBS: Os proximos passos para testar os dispositivos são iguais aos descritos no documento
 "descrição do dispositivo.pdf", abra o documento para ter um auxilio visual.
------------------------------------------------------------------------------------------------
Assim que a mensagem "Conectado!!" aparecer na saida serial precione o botão PushButton do 

dispositivo até que a mensagem “o Botão 1 foi pressionado!!” apareça, 

isso pode demorar uns 20 segundos.

Assim que passar o próximo looping o protocolo MQTT poderá observar que o LED ring irá brilhar
na cor azul, como um indicativo de que o timer do dispositivo começou

O LedRing irá mudar de cor conforme o cronometro for rodando, indicando para o usuário um estimulo visual para a passagem de tempo 
Azul: menos de 2 minutos
Verde: para entre 2 e 3minutos
Amarelo para entre 3 e 4 minutos

Laranja para entre 4 e 5 minutos
Vermelho para entre 5 e 6 minutos
 E piscará com vermelho e preto para entre 6 e 7 minutos
O na faixa de tempo entre 5 e 7 minutos o Buzzer irá começar a tocar como um alerta auditivo de o sistema esta prestes a ser desligado.

Ao chegar em a 7 minutos todos os elementos do sistema permaneceram dormentes até que o botão seja novamente pressionado

Se o cliente quiser desligar o sistema manualmente basta, novamente, manter o botão pressionado até que a mensagem “o Botão 1 foi pressionado!!” apareça novamente

Utilizando o protocolo MQTT informações sobre a utilização serão disponibilizadas através do Node-red

Sendo que as ultimas informações sobre utilização do dispositivo serão permaneceram a vista

O informações sobre o tempo em que o dispositivo permaneceu ligado também podem ser encontradas através de nosso dashboard. 

Onde poderá ser observado o tempo o ultimo banho tomado, e tempo do banhos tomados na ultima hora.



   

 