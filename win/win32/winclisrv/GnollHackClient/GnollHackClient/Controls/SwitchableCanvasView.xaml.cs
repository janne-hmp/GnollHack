﻿using GnollHackClient.Pages.Game;
using GnollHackCommon;
using SkiaSharp;
using SkiaSharp.Views.Forms;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace GnollHackClient.Controls
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class SwitchableCanvasView : ContentView
    {
        private object glLock = new object();
        private bool _useGL = false;
        public bool UseGL 
        {   get { lock (glLock) { return _useGL; } }
            set
            {
                lock(glLock)
                {
                    _useGL = value;
                    internalCanvasView.IsVisible = !_useGL;
                    internalGLView.IsVisible = _useGL;
                }
            }
        }

        public SKSize CanvasSize { get { return UseGL ? internalGLView.CanvasSize : internalCanvasView.CanvasSize; } }
        public bool IgnorePixelScaling
        {
            get { return UseGL ? false : internalCanvasView.IgnorePixelScaling; }
            set
            {
                if (!UseGL)
                    internalCanvasView.IgnorePixelScaling = value;
            }
        }
        public bool EnableTouchEvents 
        { 
            get { return UseGL ? internalGLView.EnableTouchEvents : internalCanvasView.EnableTouchEvents; } 
            set 
            {
                if (UseGL)
                    internalGLView.EnableTouchEvents = value;
                else
                    internalCanvasView.EnableTouchEvents = value;
            }
        }
        public event EventHandler<SKPaintSurfaceEventArgs> PaintSurface;
        public event EventHandler<SKTouchEventArgs> Touch;
        public void InvalidateSurface()
        {
            if(UseGL)
                internalGLView.InvalidateSurface();
            else
                internalCanvasView.InvalidateSurface();
        }
        protected override SizeRequest OnMeasure(double widthConstraint, double heightConstraint)
        {
            return new SizeRequest();
        }

        public SwitchableCanvasView()
        {
            InitializeComponent();
        }

        private void internalCanvasView_PaintSurface(object sender, SKPaintSurfaceEventArgs e)
        {
            PaintSurface?.Invoke(sender, e);
        }

        private void internalCanvasView_Touch(object sender, SKTouchEventArgs e)
        {
            Touch?.Invoke(sender, e);
        }

        private void internalGLView_PaintSurface(object sender, SKPaintGLSurfaceEventArgs e)
        {
            SKImageInfo info = new SKImageInfo();
            info.ColorType = e.ColorType;
            SKPaintSurfaceEventArgs convargs = new SKPaintSurfaceEventArgs(e.Surface, info);
            PaintSurface?.Invoke(sender, convargs);
        }

        private void internalGLView_Touch(object sender, SKTouchEventArgs e)
        {
            Touch?.Invoke(sender, e);
        }




        public GamePage _gamePage;

        public GHWindow GHWindow { get; set; }
        public ghmenu_styles MenuStyle { get; set; }
        public object MenuItemLock = new object();

        private ObservableCollection<GHMenuItem> _GHMenuItems = null;
        public ObservableCollection<GHMenuItem> MenuItems { get { return _GHMenuItems; } set { _GHMenuItems = value; } }

        public object TextItemLock = new object();
        private List<GHPutStrItem> _GHPutStrItems = null;
        public List<GHPutStrItem> PutStrItems { get { return _GHPutStrItems; } set { _GHPutStrItems = value; } }

        public SelectionMode SelectionHow { get; set; }
        public int SelectionIndex { get; set; }

        public bool RevertBlackAndWhite { get; set; }
        public bool UseTextOutline { get; set; }

        public CanvasTypes CanvasType { get; set; }


        public BindableProperty GeneralAnimationCounterProperty =
            BindableProperty.Create(nameof(GeneralAnimationCounter), typeof(long), typeof(SwitchableCanvasView), 0L);

        public long GeneralAnimationCounter
        {
            get { return (long)GetValue(GeneralAnimationCounterProperty); }
            set { SetValue(GeneralAnimationCounterProperty, value); }
        }

        protected override void OnPropertyChanged(string propertyName = null)
        {
            base.OnPropertyChanged(propertyName);

            if (_gamePage != null)
            {
                switch (CanvasType)
                {
                    case CanvasTypes.MainCanvas:
                        {
                            bool refresh = true;
                            lock (_gamePage.RefreshScreenLock)
                            {
                                refresh = _gamePage.RefreshScreen;
                            }

                            _gamePage.IncrementCounters();

                            if (refresh)
                                InvalidateSurface();

                            _gamePage.UpdateMenuAndTextCanvases();
                            break;
                        }
                    case CanvasTypes.CommandCanvas:
                        {
                            _gamePage.UpdateCommandCanvas();
                            break;
                        }
                    default:
                        break;
                }
            }
        }
    }
}