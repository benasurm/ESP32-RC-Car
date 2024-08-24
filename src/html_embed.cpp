#include "../include/FrontEnd/html_embed.h"

char const *root_html = R"raw(<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>RC Car Web Form</title>
        <style>
            body
            {
                background-color: #f0f0f0;
            }

            .SuperParent
            {
                margin: 2%;
                display: flex;
                flex-direction: column;
                height: fit-content;
            }

            .ColumnElem
            {
                margin: 0;
                display: flex;
                align-items: center;
                height: 50%;
                width: 100%;
            }

            .CameraContainer
            {
                width: fit-content;
                height: fit-content;
                display: flex;
                margin: 2%;
                column-gap: 2%;
            }

            .VideoContainer
            {
                width: 480px;
                height: 520px;
                background-color: black;
                flex-basis: 640px;
                display: flex;
                flex-direction: column;
            }

            .CaptureButton
            {
                height: 20%;
            }

            .CaptureButtonContainer
            {
                display: flex;
                justify-content: center;
                align-items: center;
            }

            .MotorControlContainer
            {
                width: 100%;
                height: fit-content;
                margin: 2%;
                display: flex;
                flex-direction: column;
                row-gap: 6%;
            }

            .ControlsHeading
            {
                font-size: 48px;
                font-family:'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            }

            .ControlHeaderContainer
            {
                width: 100%;
                height: fit-content;
            }
            
            .JoystickAndValueContainer
            {
                width: 60%;
                height: fit-content;
                display: flex;
            }

            .JoystickContainer
            {
                width: 200px;
                height: 200px;
            }
            
            .ValueContainer
            {
                width: 60%;
                height: fit-content;
                margin-left: 1%;
            }

            .AxisLabelContainer
            {
                width: 100%;
                height: 100px;
                display: flex;
            }

            .Label
            {
                width: 50%;
                height: 100%;
                display: flex;
                font-size: 32px;
                font-family:"DejaVu Sans Mono";
                margin-left: 2%;
                align-items: center;
            }

            .GenericLabel
            {
                justify-content: center;
            }

            .ControlButtonContainer
            {
                width: 96%;
                height: 20%;
                display: flex;
                margin-top: 1%;
            }

            .StopButton
            {
                width: 20%;
                height: 60px;
            }

            #FPSText
            {
                width: 100%;
                height: fit-content;
                text-align: left;
                justify-content: center;
                color: white;
                font-size: 32px;
                margin-left: 10px;
                font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            }
        </style>
        <script>
            const max_frame_count = 3;
            var frame_count = 0;
            var total_ms = 0;
            var curr_fps = -1;

            function IsUndefinedOrNull(elem)
            {
                return !(typeof(elem) != 'undefined' && elem != null);
            }

            function UpdateValueLabels(x_value, y_value)
            {
                var x_label = document.getElementById("x_value_label");
                var y_label = document.getElementById("y_value_label");

                if(IsUndefinedOrNull(x_label) || IsUndefinedOrNull(x_label))
                {
                    throw "** Selected DOM elements are not defined";
                }

                x_label.innerHTML = x_value;
                y_label.innerHTML = y_value;
            }

            function MotorSpeedRequest(x_value, y_value)
            {
                // TODO: make changes so this function can take JoyStick values as input
                if(IsUndefinedOrNull(x_value) || IsUndefinedOrNull(y_value))
                {
                    throw "** Undefined values passed as arguments";
                }
                const motor_post_req = fetch(
                    '/motor_axis',
                    {
                        method: 'POST',
                        headers: { 'Content-Type': 'text/plain' },
                        body: 'x=' + x_value + '&y=' + y_value
                    }
                );
                motor_post_req
                    .then((response) => 
                    {
                        if(!response.ok)
                        {
                            console.error('** Error in HTTP POST request: ' + response.status);
                        }
                        return response.statusText;
                    })
                    .then((data) =>
                    {
                        //console.log(data);
                    })
                    .catch((error) =>
                    {
                        console.error("** Failed to do HTTP POST request: " + error);
                    })
            }

            function FetchJPGCapture(option)
            {
                var domain = window.location.origin;
                var port = 81;
                var url = domain + ":" + port + "/jpg_capture";
                var conn_fail = 0;
                const jpg_capture_req = fetch(
                    url,
                    {
                        method: 'GET'   
                    }
                );
                var begin_time = Date.now();
                jpg_capture_req
                    .then((response) =>
                    {
                        if(!response.ok)
                        {
                            console.error("** Error in HTTP GET request: " + response.status);
                            return Promise.reject("HTTP Error");
                        }
                        const content_type = response.headers.get('Content-Type');
                        if(!content_type || !content_type.includes('image/jpeg'))
                        {
                            console.error("** HTTP JPEG GET invalid response type: {$content_type}");
                            return Promise.reject("Invalid content type");
                        }
                        return response.blob();
                    })
                    .then((data) =>
                    {
                        const image_url = window.URL.createObjectURL(data);
                        const camera_dom_obj = document.getElementById("CameraStream");
                        if(!camera_dom_obj || camera_dom_obj.nodeName !== "CANVAS")
                        {
                            console.error("** Wrong HTML element selected for stream display");
                        }
                        else
                        {
                            camera_dom_obj.dataset.URL = image_url;
                            // Option possible values:
                            // 0 - for downloading JPG capture
                            // 1 - for streaming JPG captures
                            if(option === 0)
                            {
                                DownloadJPG(camera_dom_obj.dataset.URL);
                            }
                            else if(option === 1)
                            {
                                DrawRotatedImg(camera_dom_obj.dataset.URL);
                                frame_count++;
                                if(frame_count >= max_frame_count)
                                {
                                    total_ms /= 1000;
                                    curr_fps = frame_count / total_ms;
                                    frame_count = 0;
                                    total_ms = 0;
                                }
                                else
                                {
                                    frame_count++;
                                    total_ms += Date.now() - begin_time;
                                }
                                UpdateFPSText(curr_fps.toFixed());
                                conn_fail = 0;
                                setTimeout(FetchJPGCapture(1), 10);
                            }
                            else
                            {
                                throw new ErrorEvent("** Wrong JPG capture fetch option");
                            }
                            setTimeout(() =>
                            {
                                window.URL.revokeObjectURL(camera_dom_obj.dataset.URL);
                            }, 100);
                        }
                    })
                    .catch((error) =>
                    {
                        conn_fail++;
                        if(conn_fail < 5)
                        {
                            setTimeout(FetchJPGCapture(1), 1000);
                        }
                        else
                        {
                            console.error("** Failed to do HTTP GET request: " + error);
                            return;
                        }
                        
                    })
            }

            function UpdateFPSText(curr_fps)
            {
                const label = document.getElementById('FPSText');
                if(!IsUndefinedOrNull(label))
                {
                    label.innerHTML = "FPS: " + curr_fps;
                }
            }

            function DrawRotatedImg(url)
            {
                console.log(url);
                const video_canvas = document.getElementById("CameraStream");
                if(video_canvas.getContext)
                {
                    var image = new Image();
                    image.src = url;

                    image.onload = () =>
                    {
                        const h = image.height;
                        const offset = (image.width - h) / 2;
                        const ctx = video_canvas.getContext("2d");

                        ctx.save();

                        ctx.translate(0, image.height);
                        ctx.rotate(-Math.PI / 2);
                        ctx.drawImage(image, offset, 0, h, h, 0, 0, h, h);    

                        ctx.restore();            
                    }
                }
                else
                {
                    console.error("** Canvas not supported by this browser.");
                }
            }

            function DownloadJPG(image_url)
            {
                var a = document.createElement("a");
                a.href = image_url;
                a.setAttribute("download", "image.jpeg");
                document.body.appendChild(a);
                a.click();
                document.body.removeChild(a);
            }

            // Implementation of JS Joystick found on GitHub
            /*
            * Name          : joy.js
            * @author       : Roberto D'Amico (Bobboteck)
            * Last modified : 09.06.2020
            * Revision      : 1.1.6
            *
            * Modification History:
            * Date         Version     Modified By     Description
            * 2021-12-21   2.0.0       Roberto D'Amico New version of the project that integrates the callback functions, while 
            *                                          maintaining compatibility with previous versions. Fixed Issue #27 too, 
            *                                          thanks to @artisticfox8 for the suggestion.
            * 2020-06-09   1.1.6       Roberto D'Amico Fixed Issue #10 and #11
            * 2020-04-20   1.1.5       Roberto D'Amico Correct: Two sticks in a row, thanks to @liamw9534 for the suggestion
            * 2020-04-03               Roberto D'Amico Correct: InternalRadius when change the size of canvas, thanks to 
            *                                          @vanslipon for the suggestion
            * 2020-01-07   1.1.4       Roberto D'Amico Close #6 by implementing a new parameter to set the functionality of 
            *                                          auto-return to 0 position
            * 2019-11-18   1.1.3       Roberto D'Amico Close #5 correct indication of East direction
            * 2019-11-12   1.1.2       Roberto D'Amico Removed Fix #4 incorrectly introduced and restored operation with touch 
            *                                          devices
            * 2019-11-12   1.1.1       Roberto D'Amico Fixed Issue #4 - Now JoyStick work in any position in the page, not only 
            *                                          at 0,0
            * 
            * The MIT License (MIT)
            *
            *  This file is part of the JoyStick Project (https://github.com/bobboteck/JoyStick).
            *	Copyright (c) 2015 Roberto D'Amico (Bobboteck).
            *
            * Permission is hereby granted, free of charge, to any person obtaining a copy
            * of this software and associated documentation files (the "Software"), to deal
            * in the Software without restriction, including without limitation the rights
            * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
            * copies of the Software, and to permit persons to whom the Software is
            * furnished to do so, subject to the following conditions:
            * 
            * The above copyright notice and this permission notice shall be included in all
            * copies or substantial portions of the Software.
            *
            * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
            * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
            * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
            * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
            * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
            * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
            * SOFTWARE.
            */

            let StickStatus =
            {
                xPosition: 0,
                yPosition: 0,
                x: 0,
                y: 0,
            };

            /**
             * @desc Principal object that draw a joystick, you only need to initialize the object and suggest the HTML container
             * @costructor
             * @param container {String} - HTML object that contains the Joystick
             * @param parameters (optional) - object with following keys:
             *  title {String} (optional) - The ID of canvas (Default value is 'joystick')
             *  width {Int} (optional) - The width of canvas, if not specified is setted at width of container object (Default value is the width of container object)
             *  height {Int} (optional) - The height of canvas, if not specified is setted at height of container object (Default value is the height of container object)
             *  internalFillColor {String} (optional) - Internal color of Stick (Default value is '#00AA00')
             *  internalLineWidth {Int} (optional) - Border width of Stick (Default value is 2)
             *  internalStrokeColor {String}(optional) - Border color of Stick (Default value is '#003300')
             *  externalLineWidth {Int} (optional) - External reference circonference width (Default value is 2)
             *  externalStrokeColor {String} (optional) - External reference circonference color (Default value is '#008000')
             *  autoReturnToCenter {Bool} (optional) - Sets the behavior of the stick, whether or not, it should return to zero position when released (Default value is True and return to zero)
             * @param callback {StickStatus} - 
             */
            var JoyStick = (function(container, parameters, callback)
            {
                parameters = parameters || {};
                var title = (typeof parameters.title === "undefined" ? "joystick" : parameters.title),
                    width = (typeof parameters.width === "undefined" ? 0 : parameters.width),
                    height = (typeof parameters.height === "undefined" ? 0 : parameters.height),
                    internalFillColor = (typeof parameters.internalFillColor === "undefined" ? "#00AA00" : parameters.internalFillColor),
                    internalLineWidth = (typeof parameters.internalLineWidth === "undefined" ? 2 : parameters.internalLineWidth),
                    internalStrokeColor = (typeof parameters.internalStrokeColor === "undefined" ? "#003300" : parameters.internalStrokeColor),
                    externalLineWidth = (typeof parameters.externalLineWidth === "undefined" ? 2 : parameters.externalLineWidth),
                    externalStrokeColor = (typeof parameters.externalStrokeColor ===  "undefined" ? "#008000" : parameters.externalStrokeColor),
                    autoReturnToCenter = (typeof parameters.autoReturnToCenter === "undefined" ? true : parameters.autoReturnToCenter);

                callback = callback || function(StickStatus) {};

                // Create Canvas element and add it in the Container object
                var objContainer = document.getElementById(container);
                
                // Fixing Unable to preventDefault inside passive event listener due to target being treated as passive in Chrome [Thanks to https://github.com/artisticfox8 for this suggestion]
                objContainer.style.touchAction = "none";

                var canvas = document.createElement("canvas");
                canvas.id = title;
                if(width === 0) { width = objContainer.clientWidth; }
                if(height === 0) { height = objContainer.clientHeight; }
                canvas.width = width;
                canvas.height = height;
                objContainer.appendChild(canvas);
                var context = canvas.getContext("2d");

                var pressed = 0; // Bool - 1=Yes - 0=No
                var circumference = 2 * Math.PI;
                var internalRadius = (canvas.width-((canvas.width/2)+10))/2;
                var maxMoveStick = internalRadius + 5;
                var externalRadius = internalRadius + 30;
                var centerX = canvas.width / 2;
                var centerY = canvas.height / 2;
                var directionHorizontalLimitPos = canvas.width / 10;
                var directionHorizontalLimitNeg = directionHorizontalLimitPos * -1;
                var directionVerticalLimitPos = canvas.height / 10;
                var directionVerticalLimitNeg = directionVerticalLimitPos * -1;
                // Used to save current position of stick
                var movedX = centerX;
                var movedY = centerY;

                // Check if the device support the touch or not
                if("ontouchstart" in document.documentElement)
                {
                    canvas.addEventListener("touchstart", onTouchStart, false);
                    document.addEventListener("touchmove", onTouchMove, false);
                    document.addEventListener("touchend", onTouchEnd, false);
                }
                else
                {
                    canvas.addEventListener("mousedown", onMouseDown, false);
                    document.addEventListener("mousemove", onMouseMove, false);
                    document.addEventListener("mouseup", onMouseUp, false);
                }
                // Draw the object
                drawExternal();
                drawInternal();

                /******************************************************
                 * Private methods
                 *****************************************************/

                /**
                 * @desc Draw the external circle used as reference position
                 */
                function drawExternal()
                {
                    context.beginPath();
                    context.arc(centerX, centerY, externalRadius, 0, circumference, false);
                    context.lineWidth = externalLineWidth;
                    context.strokeStyle = externalStrokeColor;
                    context.stroke();
                }

                /**
                 * @desc Draw the internal stick in the current position the user have moved it
                 */
                function drawInternal()
                {
                    context.beginPath();
                    if(movedX<internalRadius) { movedX=maxMoveStick; }
                    if((movedX+internalRadius) > canvas.width) { movedX = canvas.width-(maxMoveStick); }
                    if(movedY<internalRadius) { movedY=maxMoveStick; }
                    if((movedY+internalRadius) > canvas.height) { movedY = canvas.height-(maxMoveStick); }
                    context.arc(movedX, movedY, internalRadius, 0, circumference, false);
                    // create radial gradient
                    var grd = context.createRadialGradient(centerX, centerY, 5, centerX, centerY, 200);
                    // Light color
                    grd.addColorStop(0, internalFillColor);
                    // Dark color
                    grd.addColorStop(1, internalStrokeColor);
                    context.fillStyle = grd;
                    context.fill();
                    context.lineWidth = internalLineWidth;
                    context.strokeStyle = internalStrokeColor;
                    context.stroke();
                }

                /**
                 * @desc Events for manage touch
                 */
                let touchId = null;
                function onTouchStart(event)
                {
                    pressed = 1;
                    touchId = event.targetTouches[0].identifier;
                }

                function onTouchMove(event)
                {
                    if(pressed === 1 && event.targetTouches[0].target === canvas)
                    {
                        movedX = event.targetTouches[0].pageX;
                        movedY = event.targetTouches[0].pageY;
                        // Manage offset
                        if(canvas.offsetParent.tagName.toUpperCase() === "BODY")
                        {
                            movedX -= canvas.offsetLeft;
                            movedY -= canvas.offsetTop;
                        }
                        else
                        {
                            movedX -= canvas.offsetParent.offsetLeft;
                            movedY -= canvas.offsetParent.offsetTop;
                        }
                        // Delete canvas
                        context.clearRect(0, 0, canvas.width, canvas.height);
                        // Redraw object
                        drawExternal();
                        drawInternal();

                        // Set attribute of callback
                        StickStatus.xPosition = movedX;
                        StickStatus.yPosition = movedY;
                        StickStatus.x = (255*((movedX - centerX)/maxMoveStick)).toFixed();
                        StickStatus.y = ((255*((movedY - centerY)/maxMoveStick))*-1).toFixed();
                        callback(StickStatus);
                    }
                }

                function onTouchEnd(event)
                {
                    if (event.changedTouches[0].identifier !== touchId) return;

                    pressed = 0;
                    // If required reset position store variable
                    if(autoReturnToCenter)
                    {
                        movedX = centerX;
                        movedY = centerY;
                    }
                    // Delete canvas
                    context.clearRect(0, 0, canvas.width, canvas.height);
                    // Redraw object
                    drawExternal();
                    drawInternal();

                    // Set attribute of callback
                    StickStatus.xPosition = movedX;
                    StickStatus.yPosition = movedY;
                    StickStatus.x = (255*((movedX - centerX)/maxMoveStick)).toFixed();
                    StickStatus.y = ((255*((movedY - centerY)/maxMoveStick))*-1).toFixed();
                    callback(StickStatus);
                }

                /**
                 * @desc Events for manage mouse
                 */
                function onMouseDown(event) 
                {
                    pressed = 1;
                }

                /* To simplify this code there was a new experimental feature here: https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent/offsetX , but it present only in Mouse case not metod presents in Touch case :-( */
                function onMouseMove(event) 
                {
                    if(pressed === 1)
                    {
                        movedX = event.pageX;
                        movedY = event.pageY;
                        // Manage offset
                        if(canvas.offsetParent.tagName.toUpperCase() === "BODY")
                        {
                            movedX -= canvas.offsetLeft;
                            movedY -= canvas.offsetTop;
                        }
                        else
                        {
                            movedX -= canvas.offsetParent.offsetLeft;
                            movedY -= canvas.offsetParent.offsetTop;
                        }
                        // Delete canvas
                        context.clearRect(0, 0, canvas.width, canvas.height);
                        // Redraw object
                        drawExternal();
                        drawInternal();

                        // Set attribute of callback
                        StickStatus.xPosition = movedX;
                        StickStatus.yPosition = movedY;
                        StickStatus.x = (255*((movedX - centerX)/maxMoveStick)).toFixed();
                        StickStatus.y = ((255*((movedY - centerY)/maxMoveStick))*-1).toFixed();
                        callback(StickStatus);
                    }
                }

                function onMouseUp(event) 
                {
                    pressed = 0;
                    // If required reset position store variable
                    if(autoReturnToCenter)
                    {
                        movedX = centerX;
                        movedY = centerY;
                    }
                    // Delete canvas
                    context.clearRect(0, 0, canvas.width, canvas.height);
                    // Redraw object
                    drawExternal();
                    drawInternal();

                    // Set attribute of callback
                    StickStatus.xPosition = movedX;
                    StickStatus.yPosition = movedY;
                    StickStatus.x = (255*((movedX - centerX)/maxMoveStick)).toFixed();
                    StickStatus.y = ((255*((movedY - centerY)/maxMoveStick))*-1).toFixed();
                    callback(StickStatus);
                }

                /******************************************************
                 * Public methods
                 *****************************************************/

                /**
                 * @desc The width of canvas
                 * @return Number of pixel width 
                 */
                this.GetWidth = function () 
                {
                    return canvas.width;
                };

                /**
                 * @desc The height of canvas
                 * @return Number of pixel height
                 */
                this.GetHeight = function () 
                {
                    return canvas.height;
                };

                /**
                 * @desc The X position of the cursor relative to the canvas that contains it and to its dimensions
                 * @return Number that indicate relative position
                 */
                this.GetPosX = function ()
                {
                    return movedX;
                };

                /**
                 * @desc The Y position of the cursor relative to the canvas that contains it and to its dimensions
                 * @return Number that indicate relative position
                 */
                this.GetPosY = function ()
                {
                    return movedY;
                };

                /**
                 * @desc Normalizzed value of X move of stick
                 * @return Integer from -100 to +100
                 */
                this.GetX = function ()
                {
                    return (255*((movedX - centerX)/maxMoveStick)).toFixed();
                };

                /**
                 * @desc Normalizzed value of Y move of stick
                 * @return Integer from -100 to +100
                 */
                this.GetY = function ()
                {
                    return ((255*((movedY - centerY)/maxMoveStick))*-1).toFixed();
                };
            });
        </script>
    </head>
    <body onload="FetchJPGCapture(1)">
        <div class="SuperParent">
            <div class="ColumnElem">
                <div class="CameraContainer">
                    <div class="VideoContainer">
                        <canvas id="CameraStream" width="480" height="480">Nothing to see here</canvas>
                        <div id="FPSText">FPS: </div>
                    </div>
                    <div class="CaptureButtonContainer">
                        <button type="button" id="CaptureButton" class="CaptureButton" onclick="FetchJPGCapture(option)">Capture</button>
                    </div>
                </div>
                <div class="MotorControlContainer">
                    <div class="ControlHeaderContainer">
                        <h1 class="ControlsHeading">Controls</h1>
                    </div>
                    <div class="JoystickAndValueContainer">
                        <div id="JoystickDIV" class="JoystickContainer"></div>
                        <div class="ValueContainer">
                            <div class="AxisLabelContainer">
                                <div class="Label GenericLabel">X Value:</div>
                                <div id="x_value_label" class="Label">0</div>
                            </div>
                            <div class="AxisLabelContainer">
                                <div class="Label GenericLabel">Y Value:</div>
                                <div id="y_value_label" class="Label">0</div>
                            </div>
                        </div>
                    </div>
                    <div class="ControlButtonContainer">
                        <button type="button" class="StopButton" id="StopButton">Disconnect from RC Car</button>
                    </div>
                </div>
            </div>
            <div id="ExtraSpace"></div>
        </div>
        <script>
            // TODO: Implement interval function to change label values and send them to server-side
            var JoyStickData = {};
            var PWMJoy = new JoyStick("JoystickDIV", { "title": "pwm_joystick" });
            var joystick_value_interval = null;
            
            function UpdatePWMValues()
            {   
                if(JoyStickData.x_value !== PWMJoy.GetX() || JoyStickData.y_value !== PWMJoy.GetY())
                {
                    JoyStickData.x_value = parseInt(PWMJoy.GetX());
                    JoyStickData.y_value = parseInt(PWMJoy.GetY());
                    UpdateValueLabels(JoyStickData.x_value, JoyStickData.y_value);
                    MotorSpeedRequest(JoyStickData.x_value, JoyStickData.y_value);
                }
            }

            setInterval(UpdatePWMValues, 100);
        </script>   
    </body>
</html>)raw";


void InitializeHTMLForm(String &html_embed)
{
    if(WiFi.status() == WL_CONNECTED)
    {
        html_embed = root_html;
    }
    else
    {
        SerialIO::PrintLn(html_init_fail_msg);
        SerialIO::Print(wifi_err_code);
        SerialIO::Print(WiFi.status());
    } 
}