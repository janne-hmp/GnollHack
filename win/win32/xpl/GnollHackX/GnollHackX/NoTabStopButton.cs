﻿using System;
using System.Collections.Generic;
using System.Text;
#if GNH_MAUI
namespace GnollHackM
#else
using Xamarin.Forms;

namespace GnollHackX
#endif
{
    public class NoTabStopButton : Button
    {
        public NoTabStopButton() : base()
        {

        }
    }
}
