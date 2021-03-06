var choice = document.getElementById("is_adafruit");
var server = document.getElementById("server");
var port = document.getElementById("port");
var campo_serve = document.getElementById("campo_server");
var campo_port = document.getElementById("campo_port");
var temperatura = document.getElementById("feed_temp");
var umidade = document.getElementById("feed_umi");
var indice = document.getElementById("feed_indice");
var indiceL = document.getElementById("indice_label");
var umiL = document.getElementById("umi_label");
var tempL = document.getElementById("temp_label");

choice.addEventListener("change", function () {
    if(this.checked){
        campo_serve.hidden = true;
        campo_port.hidden = true;
        this.setAttribute('value', 'true');
        server.required = false;
        port.required = false;
    }
    else {
        campo_serve.hidden = false;
        campo_port.hidden = false;
        this.setAttribute('value', 'false');
        server.required = true;
        port.required = true;
    }
});

temperatura.addEventListener("input", function (e){
    let value = e.target.value;
    if(value == umidade.value || value == indice.value){
        this.style.color = "#FF0F0F";
        this.style.borderColor = "#FF0F0F";
        tempL.style.color = "#FF0F0F";
        tempL.innerText = "O feed de temperatura não pode ser igual ao de umidade/indice!"
        
    } else {
        this.style.color = "#004C4C";
        this.style.borderColor = "#004C4C";
        tempL.style.color = "#FFF";
        tempL.innerText = "Feed temperatura - ex:temperatura"
    }
});

umidade.addEventListener("input", function (e){
    let value = e.target.value;
    if(value == temperatura.value || value == indice.value){
        this.style.color = "#FF0F0F";
        this.style.borderColor = "#FF0F0F";
        umiL.style.color = "#FF0F0F";
        umiL.innerText = "O feed de umidade não pode ser igual ao de temperatura/indice!"
        
    } else {
        this.style.color = "#004C4C";
        this.style.borderColor = "#004C4C";
        umiL.style.color = "#FFF";
        umiL.innerText = "Feed umidade - ex:umidade"
    }
});

indice.addEventListener("input", function (e){
    let value = e.target.value;
    if(value == umidade.value || value == temperatura.value){
        this.style.color = "#FF0F0F";
        this.style.borderColor = "#FF0F0F";
        indiceL.style.color = "#FF0F0F";
        indiceL.innerText = "O feed de indice não pode ser igual ao de temperatura/umidade!"
        
    } else {
        this.style.color = "#004C4C";
        this.style.borderColor = "#004C4C";
        indiceL.style.color = "#FFF";
        indiceL.innerText = "Feed indice de calor - ex:indice"
    }
});
