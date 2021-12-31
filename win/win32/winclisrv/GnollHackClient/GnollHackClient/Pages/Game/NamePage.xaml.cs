﻿using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace GnollHackClient.Pages.Game
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class NamePage : ContentPage
    {
        public Regex ValidationExpression { get; set; }
        private ClientGame _clientGame;
        private GamePage _gamePage;

        public NamePage(GamePage gamepage)
        {
            InitializeComponent();
            ValidationExpression = new Regex(@"^[A-Za-z0-9_]{1,32}$");
            _clientGame = gamepage.ClientGame;
            _gamePage = gamepage;
        }

        private async void btnOK_Clicked(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(eName.Text))
            {
                lblError.TextColor = Color.Red;
                lblError.Text = "Please enter a name.";
                return;
            }

            if (!ValidationExpression.IsMatch(eName.Text))
            {
                lblError.TextColor = Color.Red;
                lblError.Text = "Name is invalid.";
                return;
            }

            //_clientGame.CharacterName = eName.Text;
            App.IsServerGame = false;
            ConcurrentQueue<GHResponse> queue;
            if (ClientGame.ResponseDictionary.TryGetValue(_clientGame, out queue))
            {
                queue.Enqueue(new GHResponse(_clientGame, GHRequestType.AskName, eName.Text));
                await _gamePage.Navigation.PopModalAsync();
            }
        }

        protected override void OnAppearing()
        {
            base.OnAppearing();

            eName.Focus();
        }

        private bool _backPressed = false;
        private async Task<bool> BackButtonPressed(object sender, EventArgs e)
        {
            if (!_backPressed)
            {
                _backPressed = true;
                //await App.Current.MainPage.Navigation.PopModalAsync();
            }
            return false;
        }

        private void ContentPage_Appearing(object sender, EventArgs e)
        {
            App.BackButtonPressed += BackButtonPressed;

        }

        private void ContentPage_Disappearing(object sender, EventArgs e)
        {
            App.BackButtonPressed -= BackButtonPressed;

        }

        private double _currentPageWidth = 0;
        private double _currentPageHeight = 0;
        protected override void OnSizeAllocated(double width, double height)
        {
            base.OnSizeAllocated(width, height);
            if (width != _currentPageWidth || height != _currentPageHeight)
            {
                _currentPageWidth = width;
                _currentPageHeight = height;

                WhatNameLabel.Margin = ClientUtils.GetHeaderMarginWithBorderWithBottom(bkgView.BorderStyle, width, height, 30.0);
            }
        }

    }
}