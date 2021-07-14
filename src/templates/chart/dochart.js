window.chartColors = {
    red: 'rgb(255, 99, 132)',
    orange: 'rgb(255, 159, 64)',
    yellow: 'rgb(255, 205, 86)',
    green: 'rgb(75, 192, 192)',
    blue: 'rgb(54, 162, 235)',
    purple: 'rgb(153, 102, 255)',
    grey: 'rgb(201, 203, 207)'
};

function process_arr(arr) {
    let watts = [];
    for (let el of arr) {
        let wattel = {};
        wattel.x = el.elapsed_s + el.elapsed_m * 60 + el.elapsed_h * 3600;
        wattel.y = el.watts;
        watts.push(wattel);
    }
    let config = {
        type: 'line',
        data: {
            datasets: [{
                label: 'Watts',
                backgroundColor: window.chartColors.red,
                borderColor: window.chartColors.red,
                data: watts,
                fill: false,
            }]
        },
        options: {
            responsive: true,
            grid: {
                zeroLineColor: 'rgba(0,255,0,1)'
            },
            plugins: {
                title:{
                    display:true,
                    text:'Chart.js Line Chart'
                },
                tooltips: {
                    mode: 'index',
                    intersect: false,
                }
            },
            hover: {
                mode: 'nearest',
                intersect: true
            },
            scales: {
                x: {
                    type: 'linear',
                    display: true,
                    title: {
                        display: true,
                        text: 'Seconds'
                    }
                },
                y: {
                    display: true,
                    title: {
                        display: true,
                        text: 'Watt'
                    }
                }
            }
        }
    };

    let ctx = document.getElementById('canvas').getContext('2d');
    new Chart(ctx, config);
}

function dochart_init() {
    let el = new MainWSQueueElement({
        msg: 'getsessionarray'
    }, function(msg) {
        if (msg.msg === 'R_getsessionarray') {
            return msg.content;
        }
        return null;
    }, 15000, 3);
    el.enqueue().then(process_arr).catch(function(err) {
        console.error('Error is ' + err);
    });
}


$(window).on('load', function () {
    dochart_init();
});