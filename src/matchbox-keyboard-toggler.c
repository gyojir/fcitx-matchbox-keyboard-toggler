/*
 * Copyright (c) 2021 gyojir
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <fcitx/module.h>
#include <fcitx/fcitx.h>
#include <fcitx/hook.h>
#include <fcitx/instance.h>
#include <X11/Xlib.h>
#include "matchbox-keyboard-remote.h"

static void* MatchboxKeyboardTogglerCreate(FcitxInstance* instance);
static void InputFocused(void* arg);
static void InputUnFocused(void* arg);
static int SendEvent(MBKeyboardRemoteOperation op);

typedef struct _FcitxMatchboxKeyboardToggler {
    FcitxInstance* owner;
} FcitxMatchboxKeyboardToggler;

FCITX_DEFINE_PLUGIN(fcitx_matchbox_keyboard_toggler, module, FcitxModule) = {
    .Create = MatchboxKeyboardTogglerCreate,
};

void* MatchboxKeyboardTogglerCreate(FcitxInstance* instance)
{
    FcitxMatchboxKeyboardToggler* module = fcitx_utils_malloc0(sizeof(FcitxMatchboxKeyboardToggler));
    module->owner = instance;
    
    FcitxIMEventHook hook;
    hook.arg = module;
    hook.func = InputFocused;
    FcitxInstanceRegisterInputFocusHook(instance, hook);
    hook.func = InputUnFocused;
    FcitxInstanceRegisterInputUnFocusHook(instance, hook);

    return module;
}

void InputFocused(void* arg)
{
    SendEvent(MBKeyboardRemoteShow);
}

void InputUnFocused(void* arg)
{
    SendEvent(MBKeyboardRemoteHide);
}

int SendEvent(MBKeyboardRemoteOperation op)
{
  Display *dsp = XOpenDisplay(NULL);

  if(dsp == NULL){
    return 1;
  }

  XEvent event;
  memset(&event, 0, sizeof(XEvent));

  Atom Atom_MB_IM_INVOKER_COMMAND = XInternAtom(dsp,
 					   "_MB_IM_INVOKER_COMMAND", False);
  event.xclient.type = ClientMessage;
  event.xclient.window = DefaultRootWindow(dsp);
  event.xclient.message_type = Atom_MB_IM_INVOKER_COMMAND;
  event.xclient.format = 32;
  event.xclient.data.l[0] = op;

  XSendEvent(dsp,
	      DefaultRootWindow(dsp),
	      False,
	      SubstructureRedirectMask | SubstructureNotifyMask,
	      &event);

  XSync(dsp, False);
  
  XCloseDisplay(dsp);
  
  return 0;
}
