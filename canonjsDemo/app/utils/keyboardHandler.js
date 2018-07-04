
'use strict'

define(function() {

    function createKeyboardHandler(){

        this.keyCodes = {
              KEY_Z : 90, KEY_W : 87
            , KEY_S : 83
            , KEY_A : 65, KEY_Q : 81
            , KEY_D : 68

            , KEY_R : 82

            , ARROW_LEFT  : 37
            , ARROW_RIGHT : 39
            , ARROW_UP    : 38
            , ARROW_DOWN  : 40
        };

        this._pressedKeys = {};       

        function handleKeyDown(event) {
            this._pressedKeys[event.keyCode] = true;
        }
        function handleKeyUp(event) {
            this._pressedKeys[event.keyCode] = false;
        }

        this._activated = false;
        this._handleKeyDown = handleKeyDown.bind(this);
        this._handleKeyUp   = handleKeyUp.bind(this);
    }

    //

    createKeyboardHandler.prototype.isPressed = function (code) {

        return this._pressedKeys[code];
    }

    //

    createKeyboardHandler.prototype.activate = function () {

        if (this._activated)
            return;

        document.addEventListener('keydown',    this._handleKeyDown);
        document.addEventListener('keyup',      this._handleKeyUp);

        this._activated = true;
    }

    createKeyboardHandler.prototype.deactivate = function () {

        if (!this._activated)
            return;

        document.removeEventListener('keydown',    this._handleKeyDown);
        document.removeEventListener('keyup',      this._handleKeyUp);

        this._activated = false;
    }

    return createKeyboardHandler;
});
