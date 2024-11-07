<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

function onButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "on", true);
    xhttp.send();
}

function offButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "off", true);
    xhttp.send();
}

setInterval(function getData()
{
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function()
    {
        if(this. readyState == 4 && this.status == 200)
        {
            document.getElementById("analogv").innerHTML = this.responseText;
        }
    };

    xhttp.open("GET", "read-analogv", true);
    xhttp.send();
}, 20);


var ctx = document.getElementById("hchart").getContext('2d')

const labels = ['1', '2', '3', '4'];

var data = {
    labels: labels,
    datasets: [
        {
            label: "ElectroCardiogramme",
            data: []
        }
    ]
}

var options = {
    resonsive: false
}

var config = {
    type: 'line',
    data: data,
    options: options
}
var hchart = new Chart(ctx, config)