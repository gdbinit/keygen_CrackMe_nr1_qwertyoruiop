//
//  main.c
//  keygen_CrackMe_nr1_qwertyoruiop
//
//  Created by reverser on 06/10/2018.
//  Copyright © 2018 reverser. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <CommonCrypto/CommonDigest.h>

int main(int argc, const char * argv[]) {
    
    printf("                                    _\n"
           "  /\\ /\\___ _   _  __ _  ___ _ __   / \\\n"
           " / //_/ _ \\ | | |/ _` |/ _ \\ '_ \\ /  /\n"
           "/ __ \\  __/ |_| | (_| |  __/ | | /\\_/\n"
           "\\/  \\/\\___|\\__, |\\__, |\\___|_| |_\\/\n"
           "           |___/ |___/\n"
           "    for CrackMe_nr1_qwertyoruiop\n\n"
           "(c) fG! - 2018 - https://reverse.put.as\n\n");
    
    /* ripped from https://github.com/0infinity/IOPlatformSerialNumber */
    CFMutableDictionaryRef matching;
    matching = IOServiceMatching("IOPlatformExpertDevice");
    io_service_t service;
    service = IOServiceGetMatchingService(kIOMasterPortDefault, matching);
    CFStringRef serialNumber;
    serialNumber = IORegistryEntryCreateCFProperty(service, CFSTR("IOPlatformSerialNumber"), kCFAllocatorDefault, 0);
    const char* str = CFStringGetCStringPtr(serialNumber,kCFStringEncodingMacRoman);
    printf("Your IOPlatformSerialNumber is: %s\n", str);
    
    /* first we need to hash the IOPlatformSerialNumber */
    unsigned char serial_SHA1[CC_SHA1_DIGEST_LENGTH] = {0};
    char serial_number_SHA1_str[CC_SHA1_DIGEST_LENGTH*2 + 1];
    
    CC_SHA1(str, (uint32_t)strlen(str), serial_SHA1);
    for (uint32_t i = 0; i < CC_SHA1_DIGEST_LENGTH; i++)
    {
        sprintf(&serial_number_SHA1_str[i*2], "%02x", serial_SHA1[i]);
    }
    serial_number_SHA1_str[CC_SHA1_DIGEST_LENGTH*2] = '\0';
    printf("Your IOPlatformSerialNumber SHA1 hash is: %s\n", serial_number_SHA1_str);
    
    /* now we need to hash the following sentence:
     * "So you're trying to reverse me! Hah. Have fun =P"
     *
     * we could have hardcoded to 6e6238043879e73875ace630afa8542b88d518b3
     */
    const char sentence_to_hash[] = "So you're trying to reverse me! Hah. Have fun =P";
    unsigned char sentence_SHA1[CC_SHA1_DIGEST_LENGTH] = {0};
    char sentence_SHA1_str[CC_SHA1_DIGEST_LENGTH*2 + 1];
    CC_SHA1(sentence_to_hash, (uint32_t)strlen(sentence_to_hash), sentence_SHA1);
    for (uint32_t i = 0; i < CC_SHA1_DIGEST_LENGTH; i++)
    {
        sprintf(&sentence_SHA1_str[i*2], "%02x", sentence_SHA1[i]);
    }
    sentence_SHA1_str[CC_SHA1_DIGEST_LENGTH*2] = '\0';
    printf("The hardcoded sentence SHA1 is: %s\n", sentence_SHA1_str);
    
    /* now we build the key out of mixing these two SHA1 hashes */
    unsigned char key[20] = { 0 };
    int counter = CC_SHA1_DIGEST_LENGTH - 1;
    while (counter >= 0)
    {
        unsigned char src_serial = serial_SHA1[counter];
        unsigned char src_sentence = sentence_SHA1[counter];
        key[counter] = src_serial ^ src_sentence;
        counter--;
    }
    
    /* the key to decrypt the strings is the secret hash */
    /* == TIP! == || To keygen, you need to know a secret hash. 1CED36375BA86C4DE17C940BF578ED68 is what you're looking for :) */
    const unsigned char secret_hash[16] = {
        0x1C, 0xED, 0x36, 0x37, 0x5B, 0xA8, 0x6C, 0x4D,
        0xE1, 0x7C, 0x94, 0x0B, 0xF5, 0x78, 0xED, 0x68
    };
    
    /* so the valid serial number will be something that XORed with the mixed key
     * gives us the secret hash
     */
    printf("\nYour valid key is: ");
    for (int x = 0; x < 16; x++)
    {
        printf("%02x", key[x] ^ secret_hash[x]);
    }
    printf("\n");
    return 0;
    
}
