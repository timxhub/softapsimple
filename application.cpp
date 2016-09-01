// Based on Particle's example at: https://docs.particle.io/reference/firmware/photon/#complete-example
// Modified to include the loopaction code
#include "Particle.h"
#include "softap_http.h"

SYSTEM_THREAD(ENABLED);

int d7led = D7;
int loopaction = -1;

struct Page
{
    const char* url;
    const char* mime_type;
    const char* data;
};

//  List of URLs. The number is its index and loopaction.
//  0. /index
//  1. /reset       Executes a reset of the Photon
//  2. /d7on        Switches D7 On
//  3. /d7off       Switches D7 Off
//  4. /beacon      Flashes the RGB led white a few times

const char index_html[]             = "OK Index. Try URLs: /reset /d7on /d7off /beacon";
const char reset_html[]             = "OK Reset/Rebooting ...";
const char d7on_html[]              = "OK D7 LED On";
const char d7off_html[]             = "OK D7 LED Off";
const char beacon_html[]            = "OK Going in to beacon mode for a few seconds";

// Page not found - sent with 404 response code
const char pagenotfound_html[] = "Error 404 Page Not Found";

Page myPages[] = {
     { "/index.html",       "text/plain", index_html      },
     { "/reset",            "text/plain", reset_html      },
     { "/d7on",             "text/plain", d7on_html       },
     { "/d7off",            "text/plain", d7off_html      },
     { "/beacon",           "text/plain", beacon_html     },
     { nullptr }
};

void myPage(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved)
{
    if (strcmp(url,"/index")==0) {
        Header h("Location: /index.html\r\n");
        cb(cbArg, 0, 301, "text/plain", &h);
        return;
    }

    // Detect which URL is being requested
    int8_t idx = 0;
    for (;;idx++) {
        Page& p = myPages[idx];
        if (!p.url) {
            idx = -1;
            break;
        }
        else if (strcmp(url, p.url)==0) {
            break;
        }
    }

    // Display page according to whether it is found or not found
    if (idx==-1) {
        cb(cbArg, 0, 404, "text/plain", nullptr);
        result->write(pagenotfound_html);
        loopaction = -1;
    }
    else {
        cb(cbArg, 0, 200, myPages[idx].mime_type, nullptr);
        result->write(myPages[idx].data);
        loopaction = idx;
    }
}

void startup_function()
{
   softap_set_application_page_handler(myPage, nullptr);
}

STARTUP( startup_function() );

void setup() {
    pinMode(d7led, OUTPUT);
    digitalWrite(d7led, LOW);
    WiFi.listen();
}

void loop() {
    switch (loopaction)
    {
        case 1:
            delay(200);
            System.reset();
            break;
        case 2:
            digitalWrite(d7led, HIGH);
            break;
        case 3:
            digitalWrite(d7led, LOW);
            break;
        case 4:
            RGB.control(true);
            for (int i=0; i< 10; i++) {
                RGB.color(255, 255, 255);
                delay(150);
                RGB.color(0, 0, 0);
                delay(150);
            }
            RGB.control(false);
            break;
        default:
            break;
    }
    loopaction = -1;
}

