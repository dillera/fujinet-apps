// YAAIL - Yet Another Apple Image Loader
// Based off of Brad's YAIL for Atari
//

#include "fujinet-network.h"
#include "yail.h"

#define HGR_SCREEN_ADDRESS ((unsigned char*)0x2000)

// char *httpbin = "n:https://httpbin.org/";
char url_buffer[128];
char result[1024];
uint8_t err = 0;
char *url;
uint16_t conn_bw;
uint8_t connected;
uint8_t conn_err;
uint8_t trans_type = OPEN_TRANS_CRLF;

char *version = "v1.3.14";
char *my_version = "v1.0.0";

void debug() {}

void setup() {
    uint8_t init_r = 0;
    bzero(url_buffer, 128);
    bzero(result, 1024);
    gotox(0);
    init_r = network_init();
    printf("init: %d, derr: %d\n", init_r, fn_device_error);
  #ifdef BUILD_APPLE2
    printf("nw: %d\n", sp_network);
  #endif
}

void start_get() {
    url = create_url("get");
    err = network_open(url, OPEN_MODE_HTTP_GET, trans_type);
    handle_err("open");
    err = network_json_parse(url);
    handle_err("parse");
}

void test_get_query(char *path) {
    err = network_json_query(url, path, result);
    handle_err("query");
    printf("j: >%s<, r: >%s<\n", path, result);
}

void end_get() {
    err = network_close(url);
    handle_err("close json");
}


char *create_url(char *method) {
    sprintf(url_buffer, "%s%s", method);
    return (char *) url_buffer;
}

void handle_err(char *reason) {
    if (err) {
        printf("Error: %d (d: %d) %s\n", err, fn_device_error, reason);
        cgetc();
        exit(1);
    }
}


// Function to load the image from a remote URL
void load_image(const char* image_url, unsigned char* image_data) {
    url = (const char *)image_url;
    err = network_open(url, OPEN_MODE_HTTP_GET, trans_type);
    handle_err("open");

    // simply read without any fancy modes. FN resets the modes to normal body after closing a connection, so we don't even have to specify the BODY mode.
    // We are only going to read first 40 bytes of the results so we can display it easily.
    // TODO: block reading so very large payloads can be read into limited memory are not yet handled, and above 512 may be broken on apple2.
    err = network_read(url, image_data, 8192);
    handle_err("simple_get");
    printf("simple read (same as GET):\n");
    // hex_dump(result, 40);

    network_close(url);
    handle_err("get:close");
}


// Function to display the image on the Apple II screen
void display_image(const unsigned char* image_data) {
    // Set the graphics mode to Hi-Res
    *(unsigned char*)0xC050 = 0;
    *(unsigned char*)0xC057 = 0;

  
    // Copy the image data to the Hi-Res screen buffer
    // Assuming the image size is 280x192 pixels (8192 bytes)
    memcpy(HGR_SCREEN_ADDRESS, image_data, 8192);  

    // Wait for a key press before exiting
    cgetc();
}


int main() {
    // URL of the remote image
    const char* image_url = "http://fujinet.diller.org/APPLE/duck.hgr";

    // Allocate memory for the image data
    // Assuming the image size is 280x192 pixels (8192 bytes)
    unsigned char* image_data = (unsigned char*)malloc(8192);  

    // Load the image from the remote URL
    load_image(image_url, image_data);

    // Display the image on the Apple II screen
    display_image(image_data);

 // Free the allocated memory
    free(image_data);

    return 0;
}