--TEST--
Hooks!
--FILE--
<?php

function foo() {
    return strlen("foo");
}

echo foo();
--EXPECTF--
3
