﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;
using SkiaSharp;
using SkiaSharp.Views.Forms;

using System.Runtime.InteropServices;
using Microsoft.AspNetCore.SignalR.Client;
using Microsoft.AspNetCore.Authentication;
using System.Buffers.Text;
using System.Text.Encodings.Web;
using System.Net.Http;
using System.IO;
using System.Threading;
using System.Diagnostics;
using GnollHackCommon;
using System.Reflection;
using GnollHackCommon.Authentication;
using Newtonsoft.Json;
using System.Net;
using System.Net.Http.Headers;

[assembly: ExportFont("diablo_h.ttf", Alias = "Diablo")]

namespace GnollHackClient
{
    public partial class MainPage : ContentPage
    {
        class Person
        {
            public Person(string name, DateTime birthday, Color favoriteColor)
            {
                this.Name = name;
                this.Birthday = birthday;
                this.FavoriteColor = favoriteColor;
            }

            public string Name { private set; get; }

            public DateTime Birthday { private set; get; }

            public Color FavoriteColor { private set; get; }
        };

        Boolean _connectionAttempted = false;
        private HubConnection _connection;
        private string _connection_status = "";
        private string _message = "";
        private string _message2 = "";
        private string _message3 = "";
        private string _message4 = "";
        private string _message5 = "";
        private int _result = 0;
        private int _result2 = 0;
        SKBitmap _background_bitmap;
        SKBitmap _logo_bitmap;
        SKTypeface _typeface;
        private IFmodService _fmodService;
        private IGnollHackService _gnollHackService;
        private bool _canClickButton = true;
        private bool _serverButtonClicked = false;
        private NavigationPage _loginNavPage = null;
        private Thread _gnhthread;

        public static bool LoginSuccessful { get; set; }

        public MainPage()
        {
            InitializeComponent();
            Assembly assembly = GetType().GetTypeInfo().Assembly;

            using (Stream stream = assembly.GetManifestResourceStream("GnollHackClient.Assets.window_background.png"))
            {
                _background_bitmap = SKBitmap.Decode(stream);
            }

            using (Stream stream = assembly.GetManifestResourceStream("GnollHackClient.Assets.gnollhack-logo-test-2.png"))
            {
                _logo_bitmap = SKBitmap.Decode(stream);
            }

            using (Stream stream = assembly.GetManifestResourceStream("GnollHackClient.Assets.diablo_h.ttf"))
            {
                if (stream != null)
                    _typeface = SKTypeface.FromStream(stream);
            }

            //firstButton.ImageSource = ImageSource.FromResource("GnollHackClient.Assets.button_normal.png", assembly);
            myImage.Source = ImageSource.FromResource("GnollHackClient.Assets.button_normal.png", assembly);
            myImage2.Source = ImageSource.FromResource("GnollHackClient.Assets.button_normal.png", assembly);


            /*
                            // Define some data.
                            List<Person> people = new List<Person>
                            {
                                new Person("Abigail", new DateTime(1975, 1, 15), Color.Aqua),
                                new Person("Bob", new DateTime(1976, 2, 20), Color.Black),
                                // ...etc.,...
                                new Person("Yvonne", new DateTime(1987, 1, 10), Color.Purple),
                                new Person("Zachary", new DateTime(1988, 2, 5), Color.Red)
                            };

                            // Create the ListView.
                            // Source of data items.
                            myListView.ItemsSource = people;
                            myListView.ItemTemplate = new DataTemplate(() =>
                                {
                                // Create views with bindings for displaying each property.
                                Label nameLabel = new Label();
                                    nameLabel.SetBinding(Label.TextProperty, "Name");

                                    Label birthdayLabel = new Label();
                                    birthdayLabel.SetBinding(Label.TextProperty,
                                        new Binding("Birthday", BindingMode.OneWay,
                                            null, null, "Born {0:d}"));

                                    BoxView boxView = new BoxView();
                                    boxView.SetBinding(BoxView.ColorProperty, "FavoriteColor");

                                // Return an assembled ViewCell.
                                return new ViewCell
                                    {
                                        View = new StackLayout
                                        {
                                            Padding = new Thickness(0, 5),
                                            Orientation = StackOrientation.Horizontal,
                                            Children =
                                                {
                                                boxView,
                                                new StackLayout
                                                {
                                                    VerticalOptions = LayoutOptions.Center,
                                                    Spacing = 0,
                                                    Children =
                                                    {
                                                        nameLabel,
                                                        birthdayLabel
                                                    }
                                                    }
                                                }
                                        }
                                    };
                                });
                        }
            */

            Device.StartTimer(TimeSpan.FromSeconds(1f / 40), () =>
            {
                canvasView.InvalidateSurface();
                return true;
            });

            _fmodService = DependencyService.Get<IFmodService>();
            _fmodService.InitializeFmod();
            _fmodService.LoadBanks();

            _gnollHackService = DependencyService.Get<IGnollHackService>();
            _gnollHackService.InitializeGnollHack();
            int res = _gnollHackService.Test1();
            _message = "GnollHack: " + res;
            int res2 = _gnollHackService.Test2();
            _message2 = "GnollHack2: " + res2;
        }

        protected void ConnectToServer()
        {
            if(App.AuthenticationCookie == null)
            {
                throw new Exception("AuthenticationCookie is null");
            }

            if (App.SelectedServer == null)
            {
                throw new Exception("SelectedServer is null");
            }

            _connection = new HubConnectionBuilder().WithUrl(App.SelectedServer.Url + "gnollhack", options =>
            {
                options.Cookies.Add(App.AuthenticationCookie);
            }).Build();

            if (_connection != null)
            {
                _connection_status = "Connection attempted";
            }
            else
            {
                _connection_status = "Connection attempt failed";
            }

            _connection.Closed += async (error) =>
            {
                _connection_status = "Connection closed";
                await Task.Delay(new Random().Next(0, 5) * 1000);
                await _connection.StartAsync();
            };

            _connection.On<string, string>("ReceiveMessage", (user, message) =>
            {
                _message = message;
            });

            _connection.On<int>("CalcResult", (result) =>
            {
                _result = result;
            });

            _connection.On<string, string>("LoginMessage", (user, message) =>
            {
                _message2 = message;
            });

            _connection.On<int>("AddNewGameResult", (result) =>
            {
                _message3 = "New Game Added: " + result;
            });

            _connection.On<bool>("GameAliveResult", (result) =>
            {
                _message4 = "Game Alive: " + result.ToString();
            });
            _connection.On<int, int>("Client_ExitHack", (hash, status) =>
            {
                _message5 = "ExitHack: Hash: " + hash + ", Status: " + status;
            });
            _connection.On<int>("Client_PlayerSelection", (hash) =>
            {
                _message5 = "PlayerSelection: Hash: " + hash;
            });
            _connection.On<GHCommandFromServer>("CommandFromServer", (command) =>
            {
                _message5 = "CommandFromServer: " + command.CommandName + ", GUID: " + command.Id.ToString();
            });
            _connection.On<Guid, int>("ResponseFromClientResult", (guid, result) =>
            {
                _message5 = "ResponseFromClientResult: " + result + ", GUID: " + guid;
            });
        }

        protected async void LoginToServer()
        {
            try
            {
                await _connection.StartAsync();

                await _connection.InvokeAsync("SendMessage",
                    "user", "My message");

                await _connection.InvokeAsync("DoCalc");

                await _connection.InvokeAsync("AddNewServerGame");
            }
            catch (Exception ex)
            {
                //Error
            }
        }

        private void canvasView_PaintSurface(object sender, SKPaintSurfaceEventArgs e)
        {
            SKImageInfo info = e.Info;
            SKSurface surface = e.Surface;
            SKCanvas canvas = surface.Canvas;

            canvas.Clear(SKColors.DarkGray);

            float x = info.Width;
            float y = info.Height;

            SKRect bkdest = new SKRect(0f, 0f, x, y);
            canvas.DrawBitmap(_background_bitmap, bkdest);

            float logo_width = (float)info.Width;
            float logo_height = (float)_logo_bitmap.Height * logo_width / ((float)_logo_bitmap.Width);
            x = (info.Width - logo_width) / 2;
            y = (float)info.Height * 0.2f;
            SKRect dest = new SKRect(x, y, x + logo_width, y + logo_height);
            canvas.DrawBitmap(_logo_bitmap, dest);

            string str = "GnollHack 4.1.0";

            // Create an SKPaint object to display the text
            SKPaint textPaint = new SKPaint
            {
                Color = SKColors.Black
            };

            if (_typeface != null)
                textPaint.Typeface = _typeface;

            // Adjust TextSize property so text is 90% of screen width
            float textWidth = textPaint.MeasureText(str);
            textPaint.TextSize = 0.65f * info.Width * textPaint.TextSize / textWidth;

            // Find the text bounds
            SKRect textBounds = new SKRect();
            textPaint.MeasureText(str, ref textBounds);

            float xText = info.Width / 2 - textBounds.MidX;
            float yText = y + logo_height + 30; // info.Height / 2 - textBounds.MidY;

            // And draw the text
            canvas.DrawText(str, xText, yText, textPaint);

            str = "Android Version";
            yText = yText + textPaint.TextSize + 5;
            textPaint.MeasureText(str, ref textBounds);
            xText = info.Width / 2 - textBounds.MidX;
            canvas.DrawText(str, xText, yText, textPaint);

            xText = 10;
            yText = 0;
            string additional_info = "";
            if(_connection == null && _connectionAttempted)
                additional_info = ", no _connection";
            else if (_connection == null)
            {
                /* Do nothing */
            }
            else if (_connection.State == HubConnectionState.Connected)
                additional_info = ", connected";
            else if (_connection.State == HubConnectionState.Connecting)
                additional_info = ", connecting";
            else if (_connection.State == HubConnectionState.Disconnected)
                additional_info = ", disconnected";
            else if (_connection.State == HubConnectionState.Reconnecting)
                additional_info = ", reconnecting";

            str = _connection_status + additional_info;
            textPaint.TextSize = 36;
            yText = yText + 50;
            canvas.DrawText(str, xText, yText, textPaint);

            str = _message;
            textPaint.TextSize = 36;
            yText = yText + 50;
            canvas.DrawText(str, xText, yText, textPaint);

            str = _message2;
            textPaint.TextSize = 36;
            yText = yText + 50;
            canvas.DrawText(str, xText, yText, textPaint);

            if (_result != 0)
            {
                str = _result.ToString();
                yText = yText + 50;
                canvas.DrawText(str, xText, yText, textPaint);
            }

            if(_result2 != 0)
            {
                str = _result2.ToString();
                yText = yText + 50;
                canvas.DrawText(str, xText, yText, textPaint);
            }

            str = _message3;
            yText = yText + 50;
            canvas.DrawText(str, xText, yText, textPaint);

            str = _message4;
            yText = yText + 50;
            canvas.DrawText(str, xText, yText, textPaint);

            str = _message5;
            yText = yText + 50;
            canvas.DrawText(str, xText, yText, textPaint);

        }

        private void localButton_Clicked(object sender, EventArgs e)
        {
            Thread t = new Thread(new ThreadStart(GNHThreadProc));
            _gnhthread = t;
            _gnhthread.Start();
        }

        protected void GNHThreadProc()
        {
            _gnollHackService.TestRunGnollHack();
        }

        private async void serverButton_Clicked(object sender, EventArgs e)
        {
            if(_canClickButton == false)
            {
                return;
            }

            _serverButtonClicked = true;
            _canClickButton = false;
            LoginSuccessful = false;

            var loginPage = new LoginPage();
            _loginNavPage = new NavigationPage(loginPage);

            await Navigation.PushAsync(_loginNavPage);
        }

        private async void ContentPage_Appearing(object sender, EventArgs e)
        {
            if(_serverButtonClicked)
            {
                _serverButtonClicked = false;
                _canClickButton = true;

                //Returns from login
                if (LoginSuccessful == false)
                {
                    //Not authenticated
                    return;
                }

                //Authenticated
                _fmodService.PlayTestSound();

                _connectionAttempted = true;
                _connection_status = "Not connected";
                _message = "Please wait...";

                if (_connection == null)
                {
                    ConnectToServer();
                }
                else if (_connection.State != HubConnectionState.Connected)
                {
                    await _connection.StopAsync();
                    ConnectToServer();
                }
                else
                {
                    _connection_status = "Connected";
                }

                if (_connection != null)
                {
                    LoginToServer();
                }
            }
        }
    }
}
