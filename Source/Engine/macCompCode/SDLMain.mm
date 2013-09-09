//
//  SDLMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 07.09.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "SDLMain.h"

/* The main class of the application, the application's delegate */
@implementation SDLMain

- (void)applicationWillTerminate:(NSNotification *)notification {
    AppTerminate();
}

/*
 * Catch document open requests...this lets us notice files when the app
 *  was launched by double-clicking a document, or when a document was
 *  dragged/dropped on the app's icon. You need to have a
 *  CFBundleDocumentsType section in your Info.plist to get this message,
 *  apparently.
 *
 * This message may be received multiple times to open several docs on launch.
 */

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename {
    //const char* temparg = [filename UTF8String];
    return false;
}


/* Called when the internal event loop has just started running */
- (void)applicationDidFinishLaunching:(NSNotification *) note {
    AppMain();
}

@end

void updateVideoMode() {
    bool fullScreen;
    screenSize[2] = [[NSScreen mainScreen] backingScaleFactor];
    const SDL_VideoInfo* videoInfo = updateVideoModeInternal(fullScreen);
    unsigned int width = prevOptionsState.videoWidth, height = prevOptionsState.videoHeight;
    
    if(fullScreen) {
        width /= prevOptionsState.videoScale;
        height /= prevOptionsState.videoScale;
    }
    
    SDL_Surface* screen = SDL_SetVideoMode(width, height, videoInfo->vfmt->BitsPerPixel, SDL_OPENGL);
    if(!screen) {
        log(error_log, "Coudn't set video mode, Quit.");
        exit(4);
    }
    
    NSWindow* window = [[NSApp windows] objectAtIndex:0];
    [window setTitle:@"Olypsum"];
    NSView* view = [[[window contentView] subviews] objectAtIndex:0];
    if(prevOptionsState.videoScale > 1)
        [view setWantsBestResolutionOpenGLSurface:true];
    if(fullScreen) {
        [window setStyleMask:0];
        [window setHasShadow:false];
        [window setFrame:NSRectFromCGRect(CGRectMake(0, 0, width, height)) display:false];
        [NSApp setPresentationOptions:NSApplicationPresentationHideDock | NSApplicationPresentationAutoHideMenuBar];
    }else
        [window setFrame:NSRectFromCGRect(CGRectMake((screenSize[0]-prevOptionsState.videoWidth/prevOptionsState.videoScale)>>1,
                                                     (screenSize[1]-prevOptionsState.videoHeight/prevOptionsState.videoScale)>>1,
                                                     prevOptionsState.videoWidth/prevOptionsState.videoScale,
                                                     prevOptionsState.videoHeight/prevOptionsState.videoScale)) display:false];
    [window becomeKeyWindow];
}

void setClipboardText(const char* str) {
    [[NSPasteboard generalPasteboard] clearContents];
    [[NSPasteboard generalPasteboard] setString:[NSString stringWithUTF8String:str] forType:NSStringPboardType];
}

std::string getClipboardText() {
    NSData* data = [[NSPasteboard generalPasteboard] dataForType:NSStringPboardType];
    if(!data) return "";
    return std::string(static_cast<const char*>([data bytes]));
}

bool hasClipboardText() {
    NSData* data = [[NSPasteboard generalPasteboard] dataForType:NSStringPboardType];
    return data;
}

void restartApplication() {
    NSDictionary *config = [NSDictionary dictionaryWithObjectsAndKeys:nil];
    NSError *error = nil;
    [[NSWorkspace sharedWorkspace] launchApplicationAtURL:[NSURL URLWithString:[[NSBundle mainBundle] executablePath]]
                                   options:NSWorkspaceLaunchNewInstance | NSWorkspaceLaunchDefault configuration:config error:&error];
    AppTerminate();
}