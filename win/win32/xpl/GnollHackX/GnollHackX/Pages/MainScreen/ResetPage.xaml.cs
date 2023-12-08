﻿using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
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
    public partial class ResetPage : ContentPage
    {
        public ResetPage()
        {
            InitializeComponent();
#if GNH_MAUI
            On<iOS>().SetUseSafeArea(true);
#else
            On<Xamarin.Forms.PlatformConfiguration.iOS>().SetUseSafeArea(true);
#endif
#if !DEBUG
            btnDownloadTestFiles.IsVisible = false;
            btnImportTestFiles.IsVisible = false;
#endif
        }

        private async void btnDeleteFiles_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Delete GnollHack Core Files?", "Are you sure to delete GnollHack core files?", "Yes", "No");
            if (answer)
            {
                GHApp.GnollHackService.ClearCoreFiles();
                GHApp.GnollHackService.InitializeGnollHack();
                btnDeleteFiles.Text = "Done";
                btnDeleteFiles.TextColor = GHColors.Red;
            }
            ResetGrid.IsEnabled = true;
        }

        private async void btnDeleteSavedGames_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Delete Saved Games?", "Are you sure to delete all saved games?", "Yes", "No");
            if (answer)
            {
                GHApp.GnollHackService.ClearSavedGames();
                btnDeleteSavedGames.Text = "Done";
                btnDeleteSavedGames.TextColor = GHColors.Red;
            }
            ResetGrid.IsEnabled = true;
        }

        private async void btnDeleteDumplogs_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Delete Top Scores?", "Are you sure to delete all top scores and associated dumplogs?", "Yes", "No");
            if (answer)
            {
                GHApp.GnollHackService.ClearTopScores();
                GHApp.GnollHackService.ClearDumplogs();
                btnDeleteDumplogs.Text = "Done";
                btnDeleteDumplogs.TextColor = GHColors.Red;
            }
            ResetGrid.IsEnabled = true;
        }

        private async void btnDeleteBones_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Delete Bones Files?", "Are you sure to delete all bones files?", "Yes", "No");
            if (answer)
            {
                GHApp.GnollHackService.ClearBones();
                btnDeleteBones.Text = "Done";
                btnDeleteBones.TextColor = GHColors.Red;
            }
            ResetGrid.IsEnabled = true;
        }

        private async void btnDeleteAllMainFiles_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Delete All Main Files?", "Are you sure to delete all files in the main directory?", "Yes", "No");
            if (answer)
            {
                GHApp.GnollHackService.ClearAllFilesInMainDirectory();
                btnDeleteAllMainFiles.Text = "Done";
                btnDeleteAllMainFiles.TextColor = GHColors.Red;
            }
            ResetGrid.IsEnabled = true;
        }

        private async void btnDeleteDownloads_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Delete Downloaded Files?", "Are you sure to delete all downloaded files on restart?", "Yes", "No");
            if (answer)
            {
                Preferences.Set("ResetExternalFiles", true);
                btnDeleteDownloads.Text = "Deletion on restart";
                btnDeleteDownloads.TextColor = GHColors.Red;
            }
            ResetGrid.IsEnabled = true;
        }

        private async void btnDeletePreferences_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Delete App Preferences?", "Are you sure to delete all app preference settings?", "Yes", "No");
            if (answer)
            {
                bool has_resetatstart = Preferences.ContainsKey("ResetAtStart");
                bool resetatstart = Preferences.Get("ResetAtStart", true);
                bool has_resetbanks = Preferences.ContainsKey("ResetExternalFiles");
                bool resetbanks = Preferences.Get("ResetExternalFiles", false);
                bool has_verid = Preferences.ContainsKey("VersionId");
                string verid = Preferences.Get("VersionId", "");
                bool has_vernum = Preferences.ContainsKey("VersionNumber");
                ulong vernum = (ulong)Preferences.Get("VersionNumber", 0L);

                Dictionary<string, string> saveverdict = new Dictionary<string, string>();
                Dictionary<string, DateTime> savetimedict = new Dictionary<string, DateTime>();
                foreach (SecretsFile sf in GHApp.CurrentSecrets.files)
                {
                    bool has_vbv = Preferences.ContainsKey("Verify_" + sf.id + "_Version");
                    string vbv = Preferences.Get("Verify_" + sf.id + "_Version", "");
                    if (has_vbv)
                        saveverdict.Add("Verify_" + sf.id + "_Version", vbv);
                    bool has_vblwt = Preferences.ContainsKey("Verify_" + sf.id + "_LastWriteTime");
                    DateTime vblwt = Preferences.Get("Verify_" + sf.id + "_LastWriteTime", DateTime.MinValue);
                    if (has_vblwt)
                        savetimedict.Add("Verify_" + sf.id + "_LastWriteTime", vblwt);
                }

                Preferences.Clear();

                Preferences.Set("FullVersion", GHApp.FullVersionMode);
                if (has_resetatstart)
                    Preferences.Set("ResetAtStart", resetatstart);
                if (has_resetbanks)
                    Preferences.Set("ResetExternalFiles", resetbanks);

                KeyValuePair<string, string>[] arr = saveverdict.ToArray<KeyValuePair<string, string>>();
                foreach (KeyValuePair<string, string> kvp in arr)
                {
                    Preferences.Set(kvp.Key, kvp.Value);

                }
                KeyValuePair<string, DateTime>[] arr2 = savetimedict.ToArray<KeyValuePair<string, DateTime>>();
                foreach (KeyValuePair<string, DateTime> kvp2 in arr2)
                {
                    Preferences.Set(kvp2.Key, kvp2.Value);

                }
                if (has_verid)
                    Preferences.Set("VersionId", verid);
                if (has_vernum)
                    Preferences.Set("VersionNumber", (long)vernum);

                btnDeletePreferences.Text = "Done";
                btnDeletePreferences.TextColor = GHColors.Red;
            }
            ResetGrid.IsEnabled = true;
        }

        private async void btnDeleteUserData_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Delete All User Data?", "Are you sure to delete all files in the " + GHConstants.UserDataDirectory + " directory?", "Yes", "No");
            if (answer)
            {
                try
                {
                    string datadir = Path.Combine(GHApp.GHPath, GHConstants.UserDataDirectory);
                    if (Directory.Exists(datadir))
                        Directory.Delete(datadir, true);

                    btnDeleteUserData.Text = "Done";
                    btnDeleteUserData.TextColor = GHColors.Red;
                }
                catch
                {
                    btnDeleteUserData.Text = "Failed";
                    btnDeleteUserData.TextColor = GHColors.Red;
                }
            }
            ResetGrid.IsEnabled = true;
        }

        private async void Button_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            await App.Current.MainPage.Navigation.PopModalAsync();
        }

        private bool _backPressed = false;
        private async Task<bool> BackButtonPressed(object sender, EventArgs e)
        {
            if (!_backPressed)
            {
                _backPressed = true;
                ResetGrid.IsEnabled = false;
                await App.Current.MainPage.Navigation.PopModalAsync();
            }
            return false;
        }

        private void ContentPage_Appearing(object sender, EventArgs e)
        {
            GHApp.BackButtonPressed += BackButtonPressed;
            ResetGrid.IsEnabled = true;
        }
        private void ContentPage_Disappearing(object sender, EventArgs e)
        {
            GHApp.BackButtonPressed -= BackButtonPressed;
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


        CancellationTokenSource _cancellationTokenSource = new CancellationTokenSource();

        private async void btnDownloadTestFiles_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();

            string url = "https://download.gnollhack.com/test-files/test-files2.zip";
            string target_path = GHApp.GHPath;
            string target_file = Path.Combine(target_path, "test-files2.zip");
            bool trouble = false;
            bool cancelled = false;
            using (var client = new HttpClientDownloadWithProgress(url, target_file, _cancellationTokenSource))
            {
                client.ProgressChanged += (totalFileSize, totalBytesDownloaded, progressPercentage) => {
                    //Do nothing currently
                };

                try
                {
                    await client.StartDownload();
                }
                catch (OperationCanceledException)
                {
                    /* Download was cancelled */
                    cancelled = true; ;
                }
                catch (Exception ex)
                {
                    if (!_cancellationTokenSource.Token.IsCancellationRequested)
                    {
                        await DisplayAlert("Download Error", "A download error occurred: " + ex.Message, "OK");
                        trouble = true;
                    }
                }

                try
                {
                    if (File.Exists(target_file))
                    {
                        if (_cancellationTokenSource.Token.IsCancellationRequested)
                            File.Delete(target_file);
                        else
                            System.IO.Compression.ZipFile.ExtractToDirectory(target_file, target_path);
                    }
                }
                catch (Exception ex)
                {
                    await DisplayAlert("File Process Error", "A file processing error occurred: " + ex.Message, "OK");
                    trouble = true;
                }

                if (trouble)
                {
                    btnDownloadTestFiles.Text = "Error";
                    btnDownloadTestFiles.TextColor = GHColors.Red;
                }
                else if (cancelled)
                {
                    btnDownloadTestFiles.Text = "Cancelled";
                    btnDownloadTestFiles.TextColor = GHColors.Red;
                }
                else
                {
                    btnDownloadTestFiles.Text = "Done";
                    btnDownloadTestFiles.TextColor = GHColors.Red;
                }
            }
            ResetGrid.IsEnabled = true;
        }

        public async Task<PermissionStatus> CheckAndRequestWritePermission()
        {
            var status = await Permissions.CheckStatusAsync<Permissions.StorageWrite>();

            if (status == PermissionStatus.Granted)
                return status;

            if (status == PermissionStatus.Denied && DeviceInfo.Platform == DevicePlatform.iOS)
            {
                // Prompt the user to turn on in settings
                // On iOS once a permission has been denied it may not be requested again from the application
                await DisplayAlert("Permission Needed", "GnollHack needs the file write permission to create a zip file. Please turn it on in Settings.", "OK");
                return status;
            }

            if (Permissions.ShouldShowRationale<Permissions.StorageWrite>())
            {
                await DisplayAlert("Permission Needed", "GnollHack needs the file write permission to create a zip file.", "OK");
            }

            status = await Permissions.RequestAsync<Permissions.StorageWrite>();

            return status;
        }

        public async Task<PermissionStatus> CheckAndRequestReadPermission()
        {
            var status = await Permissions.CheckStatusAsync<Permissions.StorageRead>();

            if (status == PermissionStatus.Granted)
                return status;

            if (status == PermissionStatus.Denied && DeviceInfo.Platform == DevicePlatform.iOS)
            {
                // Prompt the user to turn on in settings
                // On iOS once a permission has been denied it may not be requested again from the application
                await DisplayAlert("Permission Needed", "GnollHack needs the file read permission to work with a zip file. Please turn it on in Settings.", "OK");
                return status;
            }

            if (Permissions.ShouldShowRationale<Permissions.StorageRead>())
            {
                await DisplayAlert("Permission Needed", "GnollHack needs the file read permission to work with a zip file.", "OK");
            }

            status = await Permissions.RequestAsync<Permissions.StorageRead>();

            return status;
        }

        private async void btnImportTestFiles_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;

            GHApp.PlayButtonClickedSound();
            await CheckAndRequestWritePermission();
            await CheckAndRequestReadPermission();
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
                                string tempdirpath = Path.Combine(gnhpath, "temp");
                                if (Directory.Exists(tempdirpath))
                                    Directory.Delete(tempdirpath, true);
                                GHApp.CheckCreateDirectory(tempdirpath);

                                string temp2dirpath = Path.Combine(gnhpath, "zip");
                                if (Directory.Exists(temp2dirpath))
                                    Directory.Delete(temp2dirpath, true);
                                GHApp.CheckCreateDirectory(temp2dirpath);

                                string ziptargetfilename = file.FileName;
                                string fulltargetpath = Path.Combine(tempdirpath, ziptargetfilename);

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
                                if (extractedfiles != null)
                                {
                                    foreach (string filestr in extractedfiles)
                                    {
                                        if (System.IO.File.Exists(filestr))
                                        {
                                            FileInfo fileInfo = new FileInfo(filestr);
                                            string finalname = Path.Combine(gnhpath, fileInfo.Name);
                                            if (System.IO.File.Exists(finalname))
                                                System.IO.File.Delete(finalname);
                                            System.IO.File.Move(filestr, finalname);
                                            nextracted++;
                                        }
                                    }
                                }

                                string[] extracteddirs = Directory.GetDirectories(temp2dirpath);
                                if (extracteddirs != null)
                                {
                                    foreach (string dirstr in extracteddirs)
                                    {
                                        if(System.IO.Directory.Exists(dirstr))
                                        {
                                            DirectoryInfo dirInfo = new DirectoryInfo(dirstr);
                                            string targetdirname = dirInfo.Name;
                                            string targetdirpath = Path.Combine(gnhpath, targetdirname);
                                            if (!Directory.Exists(targetdirpath))
                                                GHApp.CheckCreateDirectory(targetdirpath);
                                            string sourcedirpath = Path.Combine(temp2dirpath, targetdirname);
                                            if (sourcedirpath == targetdirpath)
                                                continue;

                                            extractedfiles = Directory.GetFiles(sourcedirpath);
                                            foreach (string filestr in extractedfiles)
                                            {
                                                if (System.IO.File.Exists(filestr))
                                                {
                                                    FileInfo fileInfo = new FileInfo(filestr);
                                                    string finalname = Path.Combine(targetdirpath, fileInfo.Name);
                                                    if (System.IO.File.Exists(finalname))
                                                        System.IO.File.Delete(finalname);
                                                    System.IO.File.Move(filestr, finalname);
                                                    nextracted++;
                                                }
                                            }
                                        }
                                    }
                                }

                                Directory.Delete(tempdirpath, true);
                                Directory.Delete(temp2dirpath, true);
                                if (extractedfiles.Length == 0)
                                    await DisplayAlert("No Files in Zip", "There are no files in \'" + ziptargetfilename + "\'.", "OK");
                                else if (nextracted > 0)
                                    await DisplayAlert("Files from Zip", nextracted + " file" + (nextracted != 1 ? "s" : "") + " from \'" + ziptargetfilename + "\' have been saved to the GnollHack directory.", "OK");
                                else
                                    await DisplayAlert("No Files in Zip", "No files from \'" + ziptargetfilename + "\' were saved to the GnollHack directory.", "OK");
                            }
                            else
                            {
                                string out_str = "";
                                if (GHApp.GnollHackService.ValidateSaveFile(file.FullPath, out out_str))
                                {
                                    string targetfilename = file.FileName + ".i";
                                    string savedirpath = Path.Combine(gnhpath, "save");
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
                await DisplayAlert("Error", "An error occurred while trying to import files: " + ex.Message, "OK");
            }
            ResetGrid.IsEnabled = true;
        }

        private async void btnDeletePostQueues_Clicked(object sender, EventArgs e)
        {
            ResetGrid.IsEnabled = false;
            GHApp.PlayButtonClickedSound();
            bool answer = await DisplayAlert("Clear All Post Queues?", "Are you sure to delete all files in the " + GHConstants.ForumPostQueueDirectory + " and " + GHConstants.XlogPostQueueDirectory + " directories?", "Yes", "No");
            if (answer)
            {
                try
                {
                    string datadir = Path.Combine(GHApp.GHPath, GHConstants.ForumPostQueueDirectory);
                    if (Directory.Exists(datadir))
                        Directory.Delete(datadir, true);

                    datadir = Path.Combine(GHApp.GHPath, GHConstants.XlogPostQueueDirectory);
                    if (Directory.Exists(datadir))
                        Directory.Delete(datadir, true);

                    btnDeleteUserData.Text = "Done";
                    btnDeleteUserData.TextColor = GHColors.Red;
                }
                catch
                {
                    btnDeleteUserData.Text = "Failed";
                    btnDeleteUserData.TextColor = GHColors.Red;
                }
            }
            ResetGrid.IsEnabled = true;

        }
    }
}