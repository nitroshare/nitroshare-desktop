$(function() {

    var $document = $(document),
        $messages = $('.messages');

    // Retrieve all messages after the specified index
    function update() {
        $.ajax({
            url: '/api/messages',
            contentType: 'application/json',
            complete: function() {
                window.setTimeout(update, 2000);
            },
            success: function(data) {
                $messages.empty();
                $.each(data.messages, function() {
                    $('<div>')
                        .addClass('message')
                        .text(this)
                        .appendTo($messages);
                });
                $document.scrollTop(10000);
            }
        });
    }

    update();

    // Find the message input box and set the appropriate handler for [enter]
    var $input = $('#input').focus().keypress(function(e) {
        if(e.which === 13) {
            $.ajax({
                type: 'POST',
                url: '/api/messages/new',
                data: JSON.stringify({message: $(this).val()}),
                contentType: 'application/json'
            });
            // Clear the contents
            $(this).val('');
            return false;
        }
    });
});
