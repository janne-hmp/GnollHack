﻿using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Net.WebRequestMethods;
#if GNH_MAUI
using GnollHackX;
using Microsoft.Maui.Controls.PlatformConfiguration;
using Microsoft.Maui.Controls.PlatformConfiguration.iOSSpecific;

namespace GnollHackM
#else
using Xamarin.Essentials;
using Xamarin.Forms;
using Xamarin.Forms.PlatformConfiguration.iOSSpecific;
using Xamarin.Forms.Xaml;

namespace GnollHackX.Pages.MainScreen
#endif
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class AboutPage : ContentPage
    {
        public AboutPage()
        {
            InitializeComponent();
#if GNH_MAUI
            On<iOS>().SetUseSafeArea(true);
#else
            On<Xamarin.Forms.PlatformConfiguration.iOS>().SetUseSafeArea(true);
#endif
        }

        private async void btnCreditsX_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            string fulltargetpath = Path.Combine(GHApp.GHPath, "xcredits");
            var displFilePage = new DisplayFilePage(fulltargetpath, "Cross-Platform Credits", 80);
            string errormsg = "";
            if (!displFilePage.ReadFile(out errormsg))
            {
                await DisplayAlert("Error Opening File", "GnollHack cannot open the xcredits file.", "OK");
            }
            else
            {
                await App.Current.MainPage.Navigation.PushModalAsync(displFilePage);
            }
            AboutGrid.IsEnabled = true;
        }

        private async void btnCreditsW_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            string fulltargetpath = Path.Combine(GHApp.GHPath, "credits");
            var displFilePage = new DisplayFilePage(fulltargetpath, "Credits", 77);
            string errormsg = "";
            if (!displFilePage.ReadFile(out errormsg))
            {
                await DisplayAlert("Error Opening File", "GnollHack cannot open the credits file.", "OK");
            }
            else
            {
                await App.Current.MainPage.Navigation.PushModalAsync(displFilePage);
            }
            AboutGrid.IsEnabled = true;
        }

        private async void btnLicense_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            string fulltargetpath = Path.Combine(GHApp.GHPath, "license");
            var displFilePage = new DisplayFilePage(fulltargetpath, "License", 78);
            string errormsg = "";
            if (!displFilePage.ReadFile(out errormsg))
            {
                await DisplayAlert("Error Opening File", "GnollHack cannot open the license file.", "OK");
            }
            else
            {
                await App.Current.MainPage.Navigation.PushModalAsync(displFilePage);
            }
            AboutGrid.IsEnabled = true;
        }
        private async void btnGitHub_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await OpenBrowser(new Uri(GHConstants.GnollHackGitHubPage));
            AboutGrid.IsEnabled = true;
        }

        public async Task OpenBrowser(Uri uri)
        {
            try
            {
                await Browser.OpenAsync(uri, BrowserLaunchMode.SystemPreferred);
            }
            catch (Exception ex)
            {
                await DisplayAlert("Cannot Open Web Page", "GnollHack cannot open the webpage at " + uri.OriginalString + ". Error: " + ex.Message, "OK");
            }
        }

        private async void btnWebPage_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await OpenBrowser(new Uri(GHConstants.GnollHackWebPage));
            AboutGrid.IsEnabled = true;
        }

        private async void btnWiki_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await OpenBrowser(new Uri(GHConstants.GnollHackWikiPage));
            AboutGrid.IsEnabled = true;
        }

        private async void btnDowngrade_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await OpenBrowser(new Uri(GHApp.IsAndroid ? GHConstants.GnollHackAndroidDowngradePage : GHApp.IsiOS ? GHConstants.GnollHackiOSDowngradePage :  GHConstants.GnollHackGeneralDowngradePage));
            AboutGrid.IsEnabled = true;
        }

        private async void btnSponsor_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await OpenBrowser(new Uri(GHConstants.GnollHackSponsorPage));
            AboutGrid.IsEnabled = true;
        }

        private async void Button_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await App.Current.MainPage.Navigation.PopModalAsync();
        }

        private bool _backPressed = false;
        private async Task<bool> BackButtonPressed(object sender, EventArgs e)
        {
            if (!_backPressed)
            {
                _backPressed = true;
                AboutGrid.IsEnabled = false;
                await App.Current.MainPage.Navigation.PopModalAsync();
            }
            return false;
        }

        private void ContentPage_Appearing(object sender, EventArgs e)
        {
            GHApp.BackButtonPressed += BackButtonPressed;
            AboutGrid.IsEnabled = true;
        }
        private void ContentPage_Disappearing(object sender, EventArgs e)
        {
            GHApp.BackButtonPressed -= BackButtonPressed;
        }

        private async void btnCrashReport_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Send Crash Report?", "This will create a zip archive of the files in your game directory and ask it to be shared further.", "Yes", "No");
            if (answer)
            {
                await GHApp.CreateCrashReport(this);
            }
            AboutGrid.IsEnabled = true;
        }

        private async void btnDumplogs_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await GHApp.CheckAndRequestWritePermission(this);
            await GHApp.CheckAndRequestReadPermission(this);
            string archive_file = "";
            try
            {
                archive_file = GHApp.CreateDumplogZipArchive();
            }
            catch (Exception ex)
            {
                await DisplayAlert("Archive Creation Failure", "GnollHack failed to create a dumplog archive: " + ex.Message, "OK");
                AboutGrid.IsEnabled = true;
                return;
            }
            try
            {
                if (archive_file != "")
                    await GHApp.ShareFile(this, archive_file, "GnollHack Dumplogs");
            }
            catch (Exception ex)
            {
                await DisplayAlert("Share File Failure", "GnollHack failed to share a dumplog archive: " + ex.Message, "OK");
                AboutGrid.IsEnabled = true;
                return;
            }
            AboutGrid.IsEnabled = true;
        }

        private async void btnViewPanicLog_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            string fulltargetpath = Path.Combine(GHApp.GHPath, "paniclog");
            var displFilePage = new DisplayFilePage(fulltargetpath, "Panic Log", 0, true);
            string errormsg;
            if (!System.IO.File.Exists(fulltargetpath))
            {
                await DisplayAlert("No Panic Log", "Panic Log does not exist.", "OK");
            }
            else if (!displFilePage.ReadFile(out errormsg))
            {
                await DisplayAlert("Error Opening File", "GnollHack cannot open the paniclog file: " + errormsg, "OK");
            }
            else
            {
                await App.Current.MainPage.Navigation.PushModalAsync(displFilePage);
            }
            AboutGrid.IsEnabled = true;
        }

        private async void btnViewGHLog_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            string fulltargetpath = Path.Combine(GHApp.GHPath, GHConstants.AppLogDirectory, GHConstants.AppLogFileName);
            var displFilePage = new DisplayFilePage(fulltargetpath, "App Log", 0, true, false, true);
            string errormsg;
            
            if (!System.IO.File.Exists(fulltargetpath))
            {
                await DisplayAlert("No App Log", "App Log does not exist.", "OK");
            }
            else if (!displFilePage.ReadFile(out errormsg))
            {
                await DisplayAlert("Error Opening File", "GnollHack cannot open the App Log file: " + errormsg, "OK");
            }
            else
            {
                await App.Current.MainPage.Navigation.PushModalAsync(displFilePage);
            }
            AboutGrid.IsEnabled = true;
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

                lblHeader.Margin = UIUtils.GetHeaderMarginWithBorder(bkgView.BorderStyle, width, height);
                CloseButton.Margin = UIUtils.GetFooterMarginWithBorder(bkgView.BorderStyle, width, height);
            }
        }

        private async void btnVersion_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            var verPage = new VersionPage(null);
            await App.Current.MainPage.Navigation.PushModalAsync(verPage);
            AboutGrid.IsEnabled = true;

        }

        private async void btnImportSavedGames_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await GHApp.CheckAndRequestWritePermission(this);
            await GHApp.CheckAndRequestReadPermission(this);
            try
            {
                FileResult file = await FilePicker.PickAsync();

                if (file != null)
                {
                    using (Stream s = await file.OpenReadAsync())
                    {
                        if (s != null)
                        {
                            string gnhpath = GHApp.GHPath;
                            if (file.FileName.EndsWith("zip", StringComparison.OrdinalIgnoreCase))
                            {
                                string savedirpath = Path.Combine(gnhpath, GHConstants.SaveDirectory);
                                GHApp.CheckCreateDirectory(savedirpath);

                                string tempdirpath = Path.Combine(gnhpath, GHConstants.SaveDirectory, "temp");
                                if (Directory.Exists(tempdirpath))
                                    Directory.Delete(tempdirpath, true);
                                GHApp.CheckCreateDirectory(tempdirpath);
    
                                string temp2dirpath = Path.Combine(gnhpath, GHConstants.SaveDirectory, "zip");
                                if (Directory.Exists(temp2dirpath))
                                    Directory.Delete(temp2dirpath, true);
                                GHApp.CheckCreateDirectory(temp2dirpath);

                                string ziptargetfilename = file.FileName;
                                string fulltargetpath = Path.Combine(tempdirpath, ziptargetfilename);
                                string fulltargetpath2 = Path.Combine(savedirpath, ziptargetfilename);
                                if(System.IO.File.Exists(fulltargetpath2))
                                    System.IO.File.Delete(fulltargetpath2);

                                using (Stream t = System.IO.File.Open(fulltargetpath, FileMode.Create))
                                {
                                    s.CopyTo(t);
                                }
                                using (ZipArchive ziparch = ZipFile.OpenRead(fulltargetpath))
                                {
                                    ziparch.ExtractToDirectory(temp2dirpath);
                                }
                                int nextracted = 0;
                                string[] extractedfiles = Directory.GetFiles(temp2dirpath);
                                if(extractedfiles != null)
                                {
                                    foreach(string filestr in extractedfiles)
                                    {
                                        if(System.IO.File.Exists(filestr))
                                        {
                                            string out_str = "";
                                            if(GHApp.GnollHackService.ValidateSaveFile(filestr, out out_str))
                                            {
                                                FileInfo fileInfo = new FileInfo(filestr);
                                                string finalname = Path.Combine(savedirpath, fileInfo.Name + ".i");
                                                if (System.IO.File.Exists(finalname))
                                                    System.IO.File.Delete(finalname);
                                                System.IO.File.Move(filestr, finalname);
                                                nextracted++;
                                                if(out_str != "" && GHApp.DebugLogMessages)
                                                    await DisplayAlert("ValidateSaveFile Messge", out_str, "OK");
                                            }
                                            else
                                            {
                                                await DisplayAlert("Invalid Save Game in Zip", "Saved game \'" + filestr + "\' is invalid: " + out_str, "OK");
                                            }
                                        }
                                    }
                                }
                                Directory.Delete(tempdirpath, true);
                                Directory.Delete(temp2dirpath, true);
                                if(extractedfiles.Length == 0)
                                    await DisplayAlert("No Files in Zip", "There are no files in \'" + ziptargetfilename + "\'.", "OK");
                                else if (nextracted > 0)
                                    await DisplayAlert("Games Saved from Zip", "Saved games from \'" + ziptargetfilename + "\' have been saved to the save directory as non-scoring imported saved games.", "OK");
                                else
                                    await DisplayAlert("No Games Saved in Zip", "No saved games from \'" + ziptargetfilename + "\' were saved to the save directory.", "OK");
                            }
                            else
                            {
                                string out_str = "";
                                if (GHApp.GnollHackService.ValidateSaveFile(file.FullPath, out out_str))
                                {
                                    string targetfilename = file.FileName + ".i";
                                    string savedirpath = Path.Combine(gnhpath, GHConstants.SaveDirectory);
                                    GHApp.CheckCreateDirectory(savedirpath);

                                    string fulltargetpath = Path.Combine(savedirpath, targetfilename);
                                    if (System.IO.File.Exists(fulltargetpath))
                                        System.IO.File.Delete(fulltargetpath);
                                    using (Stream t = System.IO.File.Open(fulltargetpath, FileMode.Create))
                                    {
                                        s.CopyTo(t);
                                    }
                                    await DisplayAlert("Game Saved", "Saved game \'" + file.FileName + "\' has been saved to the save directory as a non-scoring imported saved game.", "OK");
                                    if (out_str != "" && GHApp.DebugLogMessages)
                                        await DisplayAlert("ValidateSaveFile Messge", out_str, "OK");
                                }
                                else
                                {
                                    await DisplayAlert("Invalid Saved Game", "Saved game \'" + file.FullPath + "\' is invalid: " + out_str, "OK");
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                await DisplayAlert("Error", "An error occurred while trying to import a saved game: " + ex.Message, "OK");
            }
            AboutGrid.IsEnabled = true;
        }

        private async void btnSavedGames_Clicked(object sender, EventArgs e)
        {
            AboutGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await GHApp.CheckAndRequestWritePermission(this);
            await GHApp.CheckAndRequestReadPermission(this);
            string archive_file = "";
            try
            {
                archive_file = GHApp.CreateSavedGamesZipArchive();
            }
            catch (Exception ex)
            {
                await DisplayAlert("Archive Creation Failure", "GnollHack failed to create a saved games archive: " + ex.Message, "OK");
                AboutGrid.IsEnabled = true;
                return;
            }
            try
            {
                if (archive_file != "")
                    await GHApp.ShareFile(this, archive_file, "GnollHack Saved Games");
            }
            catch (Exception ex)
            {
                await DisplayAlert("Share File Failure", "GnollHack failed to share a saved games archive: " + ex.Message, "OK");
                AboutGrid.IsEnabled = true;
                return;
            }
            AboutGrid.IsEnabled = true;
        }
    }
}