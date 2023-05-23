#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>

<head>
    <title>Med Dispenser</title>

    <style>
        /* general styling */
        #plus {
            width: 100%;
            padding: 0;
            outline: 0;
            border: 0;
            color: #aaa;
            background: transparent;
            cursor: pointer;
            font-size: 20px;
            font-family: FontAwesome, sans-serif;
        }

        #minus {
            width: 100%;
            padding: 0;
            outline: 0;
            border: 0;
            color: #aaa;
            background: transparent;
            cursor: pointer;
            font-size: 20px;
            font-family: FontAwesome, sans-serif;
        }

        #pm {
            position: absolute;
            top: 70px;
            left: 20px;
            opacity: .1;
            z-index: 9999;
        }

        .buttons {
            width: 300px;
            margin: 20px auto;
            text-align: center;
        }

        #result {
            padding: 20px 0 0;
            font-size: 40px;
            line-height: 10px;
        }

        #sec {
            font-size: 14px;
        }

        #start,
        #stop {
            position: relative;
            width: 100px;
            height: 40px;
            outline: 0;
            border: 0;
            border-radius: 4px;
            color: #fff;
            cursor: pointer;
            font-weight: 700;
        }

        #start {
            background: #4897F0;
        }

        #start:hover {
            background: #4087d8;
        }

        /* general styling */
        .container {
            color: #333;
            margin: 0 auto;
            text-align: center;
        }

        h1 {
            font-weight: normal;
            letter-spacing: .125rem;
            text-transform: uppercase;
        }

        li {
            display: inline-block;
            font-size: 1.5em;
            list-style-type: none;
            padding: 1em;
            text-transform: uppercase;
        }

        li span {
            display: block;
            font-size: 3.0rem;
        }

        #set_hours,
        #set_minutes,
        #set_seconds {
            display: block;
            font-size: 1.5rem;
        }

        @media all and (max-width: 768px) {
            h1 {
                font-size: calc(1.5rem * var(--smaller));
            }

            li {
                font-size: calc(1.125rem * var(--smaller));
            }

            li span {
                font-size: calc(3.375rem * var(--smaller));
            }
        }

        /* general styling */
        body {
            background: #ccc;
        }

        .dial {
            position: relative;
            width: 350px;
            height: 350px;
            background: #ececec;
            border: 12px solid #5c5c5c;
            border-radius: 50%;
            margin: 8% auto 0;
            box-shadow: 1px 14px 21px 0 rgba(0, 0, 0, .2);
        }

        span {
            display: inline-block;
        }

        .number {
            position: absolute;
            font-family: Lato, sans-serif;
            font-size: 32px;
            color: #777;
        }

        .number-12 {
            top: 15px;
            left: 155px;
        }

        .number-1 {
            top: 38px;
            right: 98px;
        }

        .number-2 {
            top: 86px;
            right: 49px;
        }

        .number-3 {
            top: 156px;
            right: 14px;
        }

        .number-4 {
            bottom: 86px;
            right: 49px;
        }

        .number-5 {
            bottom: 38px;
            right: 98px;
        }

        .number-6 {
            left: 165px;
            bottom: 15px;
        }

        .number-7 {
            bottom: 38px;
            left: 98px;
        }

        .number-8 {
            bottom: 86px;
            left: 49px;
        }

        .number-9 {
            top: 156px;
            left: 14px;
        }

        .number-10 {
            top: 86px;
            left: 40px;
        }

        .number-11 {
            top: 38px;
            left: 86px;
        }
    </style>

</head>

<body>
    <div class="dial">
        <a href="stack1" target="clockFrame"><span class="number number-1">1</span></a>
        <a href="stack2" target="clockFrame"><span class="number number-2">2</span></a>
        <a href="stack3" target="clockFrame"><span class="number number-3">3</span></a>
        <a href="stack4" target="clockFrame"><span class="number number-4">4</span></a>
        <a href="stack5" target="clockFrame"><span class="number number-5">5</span></a>
        <a href="stack6" target="clockFrame"><span class="number number-6">6</span></a>
        <a href="stack7" target="clockFrame"><span class="number number-7">7</span></a>
        <a href="stack8" target="clockFrame"><span class="number number-8">8</span></a>
        <a href="stack9" target="clockFrame"><span class="number number-9">9</span></a>
        <a href="stack10" target="clockFrame"><span class="number number-10">10</span></a>
        <a href="stack11" target="clockFrame"><span class="number number-11">11</span></a>
        <a href="stack12" target="clockFrame"><span class="number number-12">12</span></a>
        <iframe name="clockFrame" width="0" height="0" frameBorder="0"></iframe>
    </div>

    <div class="container">
        <div id="countdown">
            <ul>
                <li><span id="hours"></span>Hours</li>
                <li><span id="minutes"></span>Minutes</li>
                <li><span id="seconds"></span>Seconds</li>
            </ul>
        </div>
        <h1 id="headline">To Take Medicine</h1>
    </div>

    <div>
        <div class="container countdown">
            <ul>
                <li><input type="button" id="plus" value="&#xf0de;" onclick="increaseHours()"><span
                        id="set_hours">6</span>Hours<input type="button" id="plus" value="&#xf0de;"
                        onclick="decreaseHours()"></li>
                <li><input type="button" id="plus" value="&#xf0de;" onclick="increaseMinutes()"><span
                        id="set_minutes">0</span>Minutes<input type="button" id="plus" value="&#xf0de;"
                        onclick="decreaseMinutes()"></li>
            </ul>

            <div class="buttons">
                <form action="/get" target="_blank">
                    <input type="button" id="start" value="START" onclick="startReminder()" />
                </form>
            </div>
        </div>
    </div>

    <script>
        var numbers = document.querySelectorAll('.number');
        var tcountdown = 3600000;
        var countDownDate = new Date().getTime()
        var now = new Date().getTime();
        var distance = _countDownDate - now;
        // var countDownDate = new Date().getTime() + tcountdown;

        for (var i = 0; i < numbers.length; i++) {
            numbers[i].addEventListener('click', function () {
                var selectedNumber = this.innerHTML;
                alert('Selected Stack: ' + selectedNumber);
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        console.log(this.responseText);
                    }
                };
                xhttp.open("GET", "setStack?stack=" + selectedNumber, true);
                xhttp.send();
            });
        }

        function increaseHours() {
            document.getElementById("set_hours").innerText = Number(document.getElementById("set_hours").innerText) + 1;
        }
        function decreaseHours() {
            document.getElementById("set_hours").innerText = Number(document.getElementById("set_hours").innerText) - 1;
        }

        function increaseMinutes() {
            document.getElementById("set_minutes").innerText = Number(document.getElementById("set_minutes").innerText) + 1;
        }
        function decreaseMinutes() {
            document.getElementById("set_minutes").innerText = Number(document.getElementById("set_minutes").innerText) - 1;
        }

        function startReminder() {
            var countdown = Number(document.getElementById("set_hours").innerText) * 3600 + Number(document.getElementById("set_minutes").innerText) * 60;
            tcountdown = countdown * 1000;
            countDownDate = new Date().getTime() + tcountdown;

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    console.log(this.responseText);
                }
            };
            xhttp.open("GET", "setTimer?sec=" + countdown, true);
            xhttp.send();
            startCountdown(countDownDate);
        }

        function startCountdown(_countDownDate) {
            const second = 1000;
            const minute = second * 60;
            const hour = minute * 60;
            const day = hour * 24;

            var x = setInterval(function () {
                document.getElementById("headline").innerText = " To Take Medicine!";
                now = new Date().getTime();
                distance = _countDownDate - now;

                document.getElementById("hours").innerText = Math.floor((distance % (day)) / (hour));
                document.getElementById("minutes").innerText = Math.floor((distance % (hour)) / (minute));
                document.getElementById("seconds").innerText = Math.floor((distance % (minute)) / second);

                //do something later when date is reached
                if (distance < 0) {
                    document.getElementById("headline").innerText = "Take Medicine!";
                    // document.getElementById("countdown").style.display = "none";
                    // document.getElementById("content").style.display = "block";
                    clearInterval(x);
                    countDownDate = new Date().getTime() + tcountdown;
                    var xhttp = new XMLHttpRequest();
                    xhttp.onreadystatechange = function () {
                        if (this.readyState == 4 && this.status == 200) {
                            console.log(this.responseText);
                        }
                    };
                    xhttp.open("GET", "nextStack?stack=" + 1, true);
                    xhttp.send();

                    startCountdown(countDownDate);
                }
                //seconds
            }, 1000)
        };
    </script>

</body>

</html>

)rawliteral";