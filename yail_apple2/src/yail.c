#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <apple2.h>

// Function to load the image from a remote URL
void load_image(const char* url) {

    url = create_url("get");
    // trans mode doesn't appear to be working, always coming back 0x9b on atari
    err = network_open(url, OPEN_MODE_HTTP_GET, trans_type);
    handle_err("open");

    // simply read without any fancy modes. FN resets the modes to normal body after closing a connection, so we don't even have to specify the BODY mode.
    // We are only going to read first 40 bytes of the results so we can display it easily.
    // TODO: block reading so very large payloads can be read into limited memory are not yet handled, and above 512 may be broken on apple2.
    err = network_read(url, result, 40);
    handle_err("simple_get");
    printf("simple read (same as GET):\n");
    hex_dump(result, 40);

    network_close(url);
    handle_err("get:close");
}


// Function to display the image on the Apple II screen
void display_image(const unsigned char* image_data) {
    // Set the graphics mode to Hi-Res
    graphics(HGR);

    // Get the address of the Hi-Res screen buffer
    unsigned char* screen_buffer = (unsigned char*)0x2000;

    // Copy the image data to the screen buffer
    memcpy(screen_buffer, image_data, 8192);  // Assuming the image size is 280x192 pixels (8192 bytes)

    // Wait for a key press before exiting
    cgetc();
}

int main() {
    // URL of the remote image
    const char* image_url = "http://fujinet.diller.org/APPLE/duck.hgr";

    // Load the image from the remote URL
    load_image(image_url);

    // Display the image on the Apple II screen
    display_image(image_data);

    return 0;
}