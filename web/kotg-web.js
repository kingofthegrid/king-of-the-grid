var KOTG = {
    print: function (e) {
        $('#systme-log').append(e + '</br>')
        console.log(e);
    },
    printErr: function (e) {
        $('#systme-log').append('<span class="text-danger">' + e + '</span><br/>')
        console.log("Error: " + e);
    },
    onRuntimeInitialized: function () {
        console.log("WASM Module Initialized");
    }
};

var bot_a_selected = false;
var bot_b_selected = false;
var bot_a_name = "";
var bot_b_name = "";

function check_trigger_game()
{
    if (!bot_a_selected)
        return;
    if (!bot_b_selected)
        return;

    $('#bots-selector').hide();
    $('#game-header').hide();
    $('#game-panel').show();

    const seed = Number($('#seed').val());

    var result;

    try
    {
        // Example: Call a C++ function to process the file
        result = KOTG.ccall("test_programs", "number", ["string", "string", "number"],
            [bot_a_name, bot_b_name, seed]);
    }
    catch ({ name, message })
    {
        $('#systme-log').append('<span class="text-danger">' + message + '</span><br/>');
        $('#game-panel-body').hide();
        return;
    }

    if (result >= 0)
    {
        if (result === 0)
        {
            $('#game-panel-title').html('<span class="text-info"><i class="bi bi-exclamation-circle-fill"></i> It\'s a draw!</span>');
        }
        else if (result === 1)
        {
            $('#game-panel-title').html('<span class="text-success"><i class="bi bi-exclamation-circle-fill"></i> First bot (' + bot_a_name + ') won!</span>');
        }
        else if (result === 2)
        {
            $('#game-panel-title').html('<span class="text-success"><i class="bi bi-exclamation-circle-fill"></i> Second bot (' + bot_b_name + ') won!</span>');
        }

        // Retrieve processed file
        const outputData = FS.readFile("/recording.txt");

        var decoder = new TextDecoder('utf8');
        var b64encoded = btoa(decoder.decode(outputData));

        const recordingDataUri = `data:application/json;base64,${b64encoded}`;

        $('#asciinema-player').html('');

        // Initialize the Asciinema player
        AsciinemaPlayer.create(
            recordingDataUri,
            $('#asciinema-player')[0],
            {
                autoplay: true,
                loop: false,
                theme: "asciinema",
                fit: "width"
            }
        );
    }
}

function load_file(reader, file_name, select_node, label_node)
{
    const data = new Uint8Array(reader.result);

    try
    {
        FS.createDataFile("/", file_name, data, true, true);
    }
    catch ({ name, message }) {
        if (message == "File exists")
        {
            // ignore
        }
        else
        {
            label_node.html('<i class="bi bi-exclamation-triangle-fill"></i> ' + message).addClass('text-danger');
            return false;
        }
    }

    select_node.text('Loaded <' + file_name + '>');

    label_node.html('<i class="bi bi-check-circle-fill"></i> Selected!').addClass('text-success');
    return true;
}

$(function ()
{
    $('#bot-a-file').on('change', function (event) {
        const fileInput = $('#bot-a-file')[0];
        if (!fileInput.files.length) {
            alert("Please select a file first!");
            return;
        }

        const file = fileInput.files[0];
        const reader = new FileReader();

        reader.onload = function () {
            if (load_file(reader, file.name, $('#bot-a-file-name'), $('#bot-a-selected')))
            {
                bot_a_name = file.name;
                bot_a_selected = true;
                check_trigger_game();
            }
        };

        reader.readAsArrayBuffer(file);
    });

    $('#bot-b-file').on('change', function (event) {
        const fileInput = $('#bot-b-file')[0];
        if (!fileInput.files.length) {
            alert("Please select a file first!");
            return;
        }

        const file = fileInput.files[0];
        const reader = new FileReader();

        reader.onload = function ()
        {
            if (load_file(reader, file.name, $('#bot-b-file-name'), $('#bot-b-selected')))
            {
                bot_b_name = file.name;
                bot_b_selected = true;
                check_trigger_game();
            }
        };

        reader.readAsArrayBuffer(file);
    });

    $.ajax({
        url: "bots/list.txt",
        method: "GET",
        success: function (data) {
            const options = data.split("\n").map(item => item.trim()).filter(item => item);

            const select_a = $("#bot-a-select");
            const select_b = $("#bot-b-select");
            select_a.empty();
            select_b.empty();

            select_a.append(`<option selected>Or select Existing</option>`);
            select_b.append(`<option selected>Or select Existing</option>`);

            options.forEach(option => {
                select_a.append(`<option value="${option}">${option}</option>`);
                select_b.append(`<option value="${option}">${option}</option>`);
            });


            $('#bot-a-select').on('change', function (event) {
                const bot_name = $('#bot-a-select').val();

                $.ajax({
                    url: "bots/" + bot_name,
                    method: "GET",
                    xhrFields: {
                        responseType: "blob"
                    },
                    success: function (data) {
                        const reader = new FileReader();

                        // When the FileReader finishes reading
                        reader.onload = function (event) {
                            if (load_file(reader, bot_name, $('#bot-a-file-name'), $('#bot-a-selected')))
                            {
                                bot_a_name = bot_name;
                                bot_a_selected = true;
                                check_trigger_game();
                            }
                        };
                        reader.readAsArrayBuffer(data);
                    },
                    error: function (xhr, status, error) {
                        $('#bot-a-selected').html('<i class="bi bi-exclamation-triangle-fill"></i> ' + error).addClass('text-danger');
                    }
                });
            });

            $('#bot-b-select').on('change', function (event) {
                const bot_name = $('#bot-b-select').val();

                $.ajax({
                    url: "bots/" + bot_name,
                    method: "GET",
                    xhrFields: {
                        responseType: "blob"
                    },
                    success: function (data) {
                        const reader = new FileReader();

                        // When the FileReader finishes reading
                        reader.onload = function (event) {
                            if (load_file(reader, bot_name, $('#bot-b-file-name'), $('#bot-b-selected')))
                            {
                                bot_b_name = bot_name;
                                bot_b_selected = true;
                                check_trigger_game();
                            }
                        };
                        reader.readAsArrayBuffer(data);
                    },
                    error: function (xhr, status, error) {
                        $('#bot-b-selected').html('<i class="bi bi-exclamation-triangle-fill"></i> ' + error).addClass('text-danger');
                    }
                });
            });

        },
        error: function () {
            alert("Failed to load file. Please check the file URL.");
        }
    });
});