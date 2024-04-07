﻿using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using GnollHackX;
using Android;
using Java.IO;
using System.IO;
#if GNH_MAUI
#else
using Xamarin.Forms;
using Xamarin.Google.Android.Play.Core.AssetPacks;
using System.Runtime.Remoting.Contexts;
using Xamarin.Google.Android.Play.Core.Review;
using Xamarin.Google.Android.Play.Core.Review.Model;
using Xamarin.Google.Android.Play.Core.Tasks;
using Xamarin.Google.Android.Play.Core.Review.Testing;
#endif
using System.Threading.Tasks;
using System.Threading;
using Android.Util;
using AndroidX.Core.App;
using Android.Content.PM;
using Android.Views.InputMethods;

#if GNH_MAUI
namespace GnollHackM
#else
[assembly: Dependency(typeof(GnollHackX.Droid.PlatformService))]
namespace GnollHackX.Droid
#endif
{
    public class PlatformService : IPlatformService
    {
        public string GetVersionString()
        {
            var context = Android.App.Application.Context;
            if (Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.Tiramisu)
            {
#pragma warning disable CA1416 // Supported on: 'android' 33.0 and later
                return context.PackageManager.GetPackageInfo(context.PackageName, PackageManager.PackageInfoFlags.Of(0L)).VersionName;
#pragma warning restore CA1416 // Supported on: 'android' 33.0 and later
            }
            else
            {
#pragma warning disable CS0618 // Type or member is obsolete
                return context.PackageManager.GetPackageInfo(context.PackageName, 0).VersionName;
#pragma warning restore CS0618 // Type or member is obsolete            }
            }
        }

        public ulong GetDeviceMemoryInBytes()
        {
            try
            {
                var activityManager = Android.App.Application.Context.GetSystemService(Activity.ActivityService) as ActivityManager;
                var memoryInfo = new ActivityManager.MemoryInfo();
                activityManager.GetMemoryInfo(memoryInfo);

                long totalRam = memoryInfo.TotalMem;

                return (ulong)totalRam;
            }
            catch
            {
                return 0;
            }
        }

        public ulong GetDeviceFreeDiskSpaceInBytes()
        {
            try
            {
                //Using StatFS
                var path = new StatFs(System.Environment.GetFolderPath(System.Environment.SpecialFolder.LocalApplicationData));
                long blockSize = path.BlockSizeLong;
                long avaliableBlocks = path.AvailableBlocksLong;
                long freeSpace = blockSize * avaliableBlocks;
                return (ulong)freeSpace;
            }
            catch
            {
                return 0;
            }
        }

        public ulong GetDeviceTotalDiskSpaceInBytes()
        {
            try
            {
                //Using StatFS
                var path = new StatFs(System.Environment.GetFolderPath(System.Environment.SpecialFolder.LocalApplicationData));
                long freeSpace = path.TotalBytes;
                return (ulong)freeSpace;
            }
            catch
            {
                return 0;
            }
        }

        public void CloseApplication()
        {
            RevertAnimatorDuration(true);
            //StopForegroundService();
#if GNH_MAUI
            Platform.CurrentActivity.Finish();
            Microsoft.Maui.Controls.Application.Current.Quit();
#else
            MainActivity.CurrentMainActivity.Finish();
#endif
        }

        public void SetStatusBarHidden(bool ishidden)
        {

        }

        public bool GetStatusBarHidden()
        {
            return true;
        }

        public float GetAnimatorDurationScaleSetting()
        {
            var resolver = Android.App.Application.Context.ContentResolver;
            var scaleName = Android.Provider.Settings.Global.AnimatorDurationScale;
            float scale = Android.Provider.Settings.Global.GetFloat(resolver, scaleName, 1.0f);
            return scale;
        }

        public float GetTransitionAnimationScaleSetting()
        {
            var resolver = Android.App.Application.Context.ContentResolver;
            var scaleName = Android.Provider.Settings.Global.TransitionAnimationScale;
            float scale = Android.Provider.Settings.Global.GetFloat(resolver, scaleName, 1.0f);
            return scale;
        }

        public float GetWindowAnimationScaleSetting()
        {
            var resolver = Android.App.Application.Context.ContentResolver;
            var scaleName = Android.Provider.Settings.Global.WindowAnimationScale;
            float scale = Android.Provider.Settings.Global.GetFloat(resolver, scaleName, 1.0f);
            return scale;
        }

        public bool IsRemoveAnimationsOn()
        {
            var scale1 = GetAnimatorDurationScaleSetting();
            var scale2 = GetTransitionAnimationScaleSetting();
            var scale3 = GetWindowAnimationScaleSetting();

            return scale1 == 0 && scale2 == 0 && scale3 == 0;
        }

        public float GetCurrentAnimatorDurationScale()
        {
            float scale = -1.0f;
            try
            {
                var classForName = JNIEnv.FindClass("android/animation/ValueAnimator");
                var methodId = JNIEnv.GetStaticMethodID(classForName, "getDurationScale", "()F");

                scale = JNIEnv.CallStaticFloatMethod(classForName, methodId);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
            return scale;
        }

        private bool _originalSet = false;
        private float _originalAnimationDurationScale = 1.0f;
        public void OverrideAnimatorDuration()
        {
            float scale = GetAnimatorDurationScaleSetting();

            if (scale == 1.0f)
                return;

            if (!_originalSet)
            {
                _originalAnimationDurationScale = scale;
                _originalSet = true;
            }

            try
            {
                var classForName = JNIEnv.FindClass("android/animation/ValueAnimator");
                var methodId = JNIEnv.GetStaticMethodID(classForName, "setDurationScale", "(F)V");

                JNIEnv.CallStaticVoidMethod(classForName, methodId, new JValue(1f));
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }
        public void RevertAnimatorDuration(bool isfinal)
        {
            if (_originalSet && _originalAnimationDurationScale != 1.0f)
            {
                try
                {
                    var classForName = JNIEnv.FindClass("android/animation/ValueAnimator");
                    var methodId = JNIEnv.GetStaticMethodID(classForName, "setDurationScale", "(F)V");

                    float usedValue = !isfinal && _originalAnimationDurationScale == 0.0f ? 0.1f : _originalAnimationDurationScale; //Make sure that the value is not set to zero upon sleep just in case, since that seems to create problems
                    JNIEnv.CallStaticVoidMethod(classForName, methodId, new JValue(usedValue));
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine(ex.Message);
                }

                _originalSet = false;
                _originalAnimationDurationScale = 1.0f;
            }
        }

#if GNH_MAUI
        public async void RequestAppReview(ContentPage page)
        {
            await Task.Delay(50);
        }
#else
        IReviewManager _manager;
        TaskCompletionSource<bool> _tcs;
        TaskCompletionSource<bool> _tcs2 = null;
        public readonly object LogLock = new object();
        public List<string> Log = new List<string>();
        public TaskCompletionSource<bool> Tcs2 { get { return _tcs2; } set { _tcs2 = value; } }

        public async void RequestAppReview(ContentPage page)
        {
            lock(LogLock)
            {
                Log.Clear();
                Log.Add("Starting App Review");
            }

            _tcs?.TrySetCanceled();
            _tcs = new TaskCompletionSource<bool>();
            _manager = ReviewManagerFactory.Create(MainActivity.CurrentMainActivity);

            var request = _manager.RequestReviewFlow();
            var listener = new StoreReviewTaskCompleteListener(_manager, _tcs, this, false);
            request.AddOnCompleteListener(listener);

            lock (LogLock)
            {
                Log.Add("Awaiting");
            }
            await _tcs.Task;
            if (_tcs2 != null)
                await _tcs2.Task;
            _manager.Dispose();
            request.Dispose();
            lock (LogLock)
            {
                Log.Add("Done");
            }

            string logs = "";
            lock (LogLock)
            {
                foreach (var log in Log)
                {
                    if (logs != "")
                        logs += ". ";
                    logs += log;
                }
                Log.Clear();
            }
            if (GHApp.DebugLogMessages)
                GHApp.MaybeWriteGHLog("App Review Log: " + logs);
        }
#endif

        public string GetBaseUrl()
        {
            return "file:///android_asset/";
        }
        public string GetAssetsPath()
        {

            return "file:///android_asset/";
        }

        public string GetCanonicalPath(string fileName)
        {
            try
            {
                Java.IO.File file = new Java.IO.File(fileName);
                return file.CanonicalPath;
            }
            catch (Exception) 
            {
                return fileName;
            }
        }

        public string GetAbsoluteOnDemandAssetPath(string assetPack)
        {
#if GNH_MAUI
            return null;
#else
            if (MainActivity.CurrentMainActivity == null || MainActivity.CurrentMainActivity.AssetPackManager == null)
                return null;

            var assetPackPath = MainActivity.CurrentMainActivity.AssetPackManager.GetPackLocation(assetPack);
            return assetPackPath?.AssetsPath() ?? null;
#endif
        }

        public string GetAbsoluteOnDemandAssetPath(string assetPack, string relativeAssetPath)
        {
#if GNH_MAUI
            return null;
#else
            if (MainActivity.CurrentMainActivity == null || MainActivity.CurrentMainActivity.AssetPackManager == null)
                return null;

            string assetsFolderPath = GetAbsoluteOnDemandAssetPath(assetPack);
            if (assetsFolderPath == null)
            {
                // asset pack is not ready
                return null;
            }

            string assetPath = Path.Combine(assetsFolderPath, relativeAssetPath);
            return assetPath;
#endif
        }

        public int FetchOnDemandPack(string pack)
        {
#if GNH_MAUI
            return 0;
#else
            if (MainActivity.CurrentMainActivity == null || MainActivity.CurrentMainActivity.AssetPackManager == null)
                return 2; /* No asset pack manager */

            var location = MainActivity.CurrentMainActivity.AssetPackManager.GetPackLocation(pack);
            if (location == null)
            {
                // TODO Figure out how to use the GetPackStates.
                try
                {
                    var states = MainActivity.CurrentMainActivity.AssetPackManager.GetPackStates(new string[] { pack });
                    // TODO add OnComplete Listeners to the Task returned by Fetch.
                    MainActivity.CurrentMainActivity.AssetPackManager.Fetch(new string[] { pack });
                    return 0;  /* Success */
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine(ex.Message);
                    return 1;  /* Exception occurred */
                }
            }
            else
                return -1; /* Already loaded */
#endif
        }

        public event EventHandler<AssetPackStatusEventArgs> OnDemandPackStatusNotification;

        private void InitOnDemandPackStatusNotificationEventHandler()
        {
#if GNH_MAUI
#else
            MainActivity.CurrentMainActivity.OnDemandPackStatus += OnDemandPackStatusNotified;
#endif
        }

        private void OnDemandPackStatusNotified(object sender, AssetPackStatusEventArgs e)
        {
            OnDemandPackStatusNotification?.Invoke(this, e);
        }

        private void StartForegroundService()
        {
            var intent = new Intent(MainActivity.CurrentMainActivity, typeof(GnhSource));

            if (Android.OS.Build.VERSION.SdkInt >= Android.OS.BuildVersionCodes.O)
            {
                MainActivity.CurrentMainActivity.StartForegroundService(intent);
            }
            else
            {
                MainActivity.CurrentMainActivity.StartService(intent);
            }
        }

        private void StopForegroundService()
        {
            var intent = new Intent(MainActivity.CurrentMainActivity, typeof(App));
            MainActivity.CurrentMainActivity.StopService(intent);
        }

        public void InitializePlatform()
        {
            InitOnDemandPackStatusNotificationEventHandler();
            //StartForegroundService();
        }

        public void HideKeyboard()
        {
            var context = Android.App.Application.Context;
            var inputMethodManager = context.GetSystemService(Android.Content.Context.InputMethodService) as InputMethodManager;
            if (inputMethodManager != null)
            {
                var activity = MainActivity.CurrentMainActivity;
                if(activity != null)
                {
                    var token = activity.CurrentFocus?.WindowToken;
                    inputMethodManager.HideSoftInputFromWindow(token, HideSoftInputFlags.None);
                    activity.Window.DecorView.ClearFocus();
                }
            }
        }

        public void HideOsNavigationBar()
        {
#if GNH_MAUI
            Activity activity = MainActivity.CurrentMainActivity;
            if (activity == null)
                MainActivity.DefaultShowNavigationBar = false;
            else
                MainActivity.HideOsNavigationBar();
#endif
        }

        public void ShowOsNavigationBar()
        {
#if GNH_MAUI
            Activity activity = MainActivity.CurrentMainActivity;
            if (activity == null)
                MainActivity.DefaultShowNavigationBar = true;
            else
                MainActivity.ShowOsNavigationBar();
#endif
        }
    }

    public interface INotification
    {
        Notification ReturnNotif();
    }

    internal class NotificationHelper : INotification
    {
        private static string foregroundChannelId = "9001";
        private static Android.Content.Context context = global::Android.App.Application.Context;

        public Notification ReturnNotif()
        {
            // Building intent
            var intent = new Intent(context, typeof(MainActivity));
            intent.AddFlags(ActivityFlags.SingleTop);
            intent.PutExtra("GnollHack", "Foreground");

            var pendingIntent = PendingIntent.GetActivity(context, 0, intent, PendingIntentFlags.UpdateCurrent);

            var notifBuilder = new NotificationCompat.Builder(context, foregroundChannelId)
                .SetContentTitle("GnollHack")
                .SetContentText("GnollHack in the Foreground")
                .SetSmallIcon(Resource.Drawable.notification_icon_background)
                .SetOngoing(true)
                .SetContentIntent(pendingIntent);

            // Building channel if API verion is 26 or above
            if (global::Android.OS.Build.VERSION.SdkInt >= BuildVersionCodes.O)
            {
                NotificationChannel notificationChannel = new NotificationChannel(foregroundChannelId, "GnollHack", NotificationImportance.High);
                notificationChannel.Importance = NotificationImportance.High;
                notificationChannel.EnableLights(true);
                notificationChannel.EnableVibration(true);
                notificationChannel.SetShowBadge(true);
                notificationChannel.SetVibrationPattern(new long[] { 100, 200, 300, 400, 500, 400, 300, 200, 400 });

                var notifManager = context.GetSystemService(Android.Content.Context.NotificationService) as NotificationManager;
                if (notifManager != null)
                {
                    notifBuilder.SetChannelId(foregroundChannelId);
                    notifManager.CreateNotificationChannel(notificationChannel);
                }
            }

            return notifBuilder.Build();
        }
    }

    public class GnhSource : Service
    {
        public override IBinder OnBind(Intent intent)
        {
            return null;
        }

        public const int ServiceRunningNotifID = 9000;

        public override StartCommandResult OnStartCommand(Intent intent, StartCommandFlags flags, int startId)
        {
            Notification notif = DependencyService.Get<INotification>().ReturnNotif();
            StartForeground(ServiceRunningNotifID, notif);

            //_ = DoLongRunningOperationThings();

            return StartCommandResult.Sticky;
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
        }

        public override bool StopService(Intent name)
        {
            return base.StopService(name);
        }
    }

#if GNH_MAUI
#else
    public class StoreReviewTaskCompleteListener : Java.Lang.Object, IOnCompleteListener
    {
        IReviewManager _manager;
        TaskCompletionSource<bool> _tcs;
        PlatformService _ps;
        bool _isLaunchReviewFlow = false;

        public StoreReviewTaskCompleteListener(IReviewManager manager, TaskCompletionSource<bool> tcs, PlatformService ps, bool isLaunchReviewFlow)
        {
            _manager = manager;
            _tcs = tcs;
            _ps = ps;
            _isLaunchReviewFlow = isLaunchReviewFlow;
        }

        Xamarin.Google.Android.Play.Core.Tasks.Task launchTask;

        public void OnComplete(Xamarin.Google.Android.Play.Core.Tasks.Task task)
        {
            if (_isLaunchReviewFlow)
            {

                lock (_ps.LogLock)
                {
                    _ps.Log.Add("OnComplete / _isLaunchReviewFlow / isSuccessful: " + task.IsSuccessful.ToString());
                }
                if (!task.IsSuccessful)
                {
                    _ps.Tcs2 = null;
                }
                _tcs?.TrySetResult(task.IsSuccessful);
                launchTask?.Dispose();
            }
            else
            {
                lock (_ps.LogLock)
                {
                    _ps.Log.Add("OnComplete / normal / isSuccessful: " + task.IsSuccessful.ToString());
                }
                if (task.IsSuccessful)
                {
                    //Launch review flow
                    try
                    {
                        if (_ps.Tcs2 != null)
                            _ps.Tcs2.TrySetCanceled();
                        else
                            _ps.Tcs2 = new TaskCompletionSource<bool>();
                        var reviewInfo = (ReviewInfo)task.GetResult(Java.Lang.Class.FromType(typeof(ReviewInfo)));
                        launchTask = _manager.LaunchReviewFlow(MainActivity.CurrentMainActivity, reviewInfo);
                        launchTask.AddOnCompleteListener(new StoreReviewTaskCompleteListener(_manager, _ps.Tcs2, _ps, true));
                        _tcs?.TrySetResult(true);
                        lock (_ps.LogLock)
                        {
                            _ps.Log.Add("OnComplete / normal / Finished");
                        }
                    }
                    catch (Exception ex)
                    {
                        _ps.Tcs2 = null;
                        _tcs?.TrySetResult(false);
                        System.Diagnostics.Debug.WriteLine(ex.Message);
                        lock (_ps.LogLock)
                        {
                            _ps.Log.Add("OnComplete: Exception: " + ex.Message);
                        }
                    }
                }
                else
                {
                    _ps.Tcs2 = null;
                    _tcs?.TrySetResult(false);
                }
            }
        }
    }
#endif
}