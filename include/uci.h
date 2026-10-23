#pragma once

// Run the UCI protocol loop, reading commands from stdin and writing responses
// to stdout. Returns when the GUI sends "quit" or closes the input stream.
int runUci();
