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

void handle_err(char *reason) {
    if (err) {
        printf("Error: %d (d: %d) %s\n", err, fn_device_error, reason);
        cgetc();
        exit(1);
    }
}

char *create_url(char *method) {
    sprintf(url_buffer, "%s%s", httpbin, method);
    return (char *) url_buffer;
}

void post(char *devicespec, char *data) {
    set_json(url);
    network_http_post(devicespec, data);
}

void body(char *devicespec, char *r, uint16_t len) {
    strcpy(r, "NO DATA");
    err = network_http_set_channel_mode(devicespec, HTTP_CHAN_MODE_BODY);
    handle_err("body chan mode");
    err = network_read(devicespec, (uint8_t *) r, len);
    handle_err("body read");
}

void set_json(char *devicespec) {
    network_http_start_add_headers(devicespec);
    network_http_add_header(devicespec, "Accept: application/json");
    network_http_add_header(devicespec, "Content-Type: application/json");
    network_http_end_add_headers(devicespec);
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