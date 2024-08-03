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
                width: 640px;
                height: 480px;
                background-color: black;
                flex-basis: 640px;
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
            }

            .SliderContainer
            {
                width: 98%;
                min-width: 500px;
                height: 30%;
                display: flex;
                column-gap: 1%;
                margin: 1%;
            }

            .GenericLabel
            {
                width: 10%;
                display: flex;
                align-items: center;
                font-size: 18px;
                font-family:"DejaVu Sans Mono";
                margin: 2%;
            }

            .LeftLabel
            {
                text-align: left;
            }

            .RightLabel
            {
                text-align: right;
            }

            .RangeSlider
            {
                width: 250px;
                height: 50px;
            }

            .ControlButtonContainer
            {
                width: 96%;
                height: 20%;
                display: flex;
                margin: 1%;
            }

            .StopButton
            {
                width: 20%;
                height: 60px;
            }
        </style>
        <script>
            function IsUndefinedOrNull(elem)
            {
                return !(typeof(elem) != 'undefined' && elem != null);
            }

            function UpdateValueLabel(rangeID, labelID)
            {
                var range_slider = document.getElementById(rangeID);
                if(!IsUndefinedOrNull(range_slider))
                {
                    var value_label = document.getElementById(labelID);
                    if(!IsUndefinedOrNull(value_label))
                    {
                        value_label.innerHTML = range_slider.value;
                    }
                    else console.log("Element by ID of: " + labelID + " was not found");
                }
                else console.log("Element by ID of: " + rangeID + " was not found");
            }

            function ChangeValueLabels(label, range_slider, class_name)
            {
                if(label.htmlFor == range_slider.id && label.classList.contains(class_name))
                    label.innerHTML = range_slider.value;
            }

            function FindAndChangeValueLabels(elem, class_name)
            {
                var ValueLabels = document.getElementsByTagName("label");
                for(var i = 0; i < ValueLabels.length; i++)
                {
                    ChangeValueLabels(ValueLabels[i], elem, class_name);
                }
            }

            function SetRangeAndLabelToZero(elem, class_name)
            {
                if(!IsUndefinedOrNull(elem))
                {
                    elem.value = 0;
                    FindAndChangeValueLabels(elem, class_name);
                }
                else console.log("Argument elem in function SetRangeAndLabelToZero() is not found.");
            }

            function SetRangeValuesToZero(input_class_name, label_class_name)
            {
                const RangeSliders = document.getElementsByClassName(input_class_name);
                if(!IsUndefinedOrNull(RangeSliders))
                {
                    for(var i = 0; i < RangeSliders.length; i++)
                    {
                        SetRangeAndLabelToZero(RangeSliders[i], label_class_name);
                    }
                    MotorSpeedRequest();
                }
            }

            function MotorSpeedRequest()
            {
                var ranges = document.getElementsByClassName("MotorSpeed");
                if(ranges.length < 2)
                {
                    throw '** Too few range arguments';
                }
                const motor_post_req = fetch(
                    '/motor_axis',
                    {
                        method: 'POST',
                        headers: { 'Content-Type': 'text/plain' },
                        body: 'x=' + ranges[1].value + '&y=' + ranges[0].value 
                    }
                );
                motor_post_req
                    .then((response) => 
                    {
                        if(!response.ok)
                        {
                            throw new Error('** Error in HTTP POST request: ' + response.status);
                        }
                        return response.statusText;
                    })
                    .then((data) =>
                    {
                        console.log(data);
                    })
                    .catch((error) =>
                    {
                        console.error("** Failed to do HTTP POST request: " + error);
                    })
            }

            function MotorSpeedChange(axis_id, value_label_id)
            {
                UpdateValueLabel(axis_id, value_label_id);
                MotorSpeedRequest();
            }
        </script>
    </head>
    <body>
        <div class="SuperParent">
            <div class="ColumnElem">
                <div class="CameraContainer">
                    <div class="VideoContainer"></div>
                    <div class="CaptureButtonContainer">
                        <button type="button" id="CaptureButton" class="CaptureButton">Capture</button>
                    </div>
                </div>
                <div class="MotorControlContainer">
                    <div class="ControlHeaderContainer">
                        <h1 class="ControlsHeading">Controls</h1>
                    </div>
                    <div class="SliderContainer">
                        <div class="GenericLabel LeftLabel">
                            <label for="vertical-axis">Backw.</label>
                        </div>
                        <input type="range" id="vertical-axis" class="RangeSlider MotorSpeed" min="-255" max="255" value="0" 
                            onchange="MotorSpeedChange('vertical-axis', 'vertical-axis-value')">
                        <div class="GenericLabel RightLabel">
                            <label for="vertical-axis">Forw.</label>
                        </div>
                        <div class="GenericLabel LeftLabel">
                            <label for="vertical-axis">Value:</label>
                        </div>
                        <div class="GenericLabel LeftLabel">
                            <label for="horizontal-axis" id="vertical-axis-value" class="SpeedValueLabel">0</label>
                        </div>
                    </div>
                    <div class="SliderContainer">
                        <div class="GenericLabel LeftLabel">
                            <label for="horizontal-axis">Left</label>
                        </div>
                        <input type="range" id="horizontal-axis" class="RangeSlider MotorSpeed" min="-255" max="255" value="0" 
                            onchange="MotorSpeedChange('horizontal-axis', 'horizontal-axis-value')">
                        <div class="GenericLabel RightLabel">
                            <label for="horizontal-axis">Right</label>
                        </div>
                        <div class="GenericLabel LeftLabel">
                            <label for="horizontal-axis">Value:</label>
                        </div>
                        <div class="GenericLabel LeftLabel">
                            <label for="horizontal-axis" id="horizontal-axis-value" class="SpeedValueLabel">0</label>
                        </div>
                    </div>
                    <div class="ControlButtonContainer">
                        <button type="button" class="StopButton" id="StopButton" onclick="SetRangeValuesToZero('MotorSpeed', 'SpeedValueLabel')">Stop</button>
                    </div>
                </div>
            </div>
            <div id="ExtraSpace"></div>
        </div>   
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