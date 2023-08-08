﻿using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;

namespace GnollHackX
{
    public interface IPlatformService
    {
        void InitializePlatform();
        void CloseApplication();

        string GetVersionString();
        ulong GetDeviceMemoryInBytes();
        ulong GetDeviceFreeDiskSpaceInBytes();
        ulong GetDeviceTotalDiskSpaceInBytes();
        void SetStatusBarHidden(bool ishidden);
        bool GetStatusBarHidden();
        float GetAnimatorDurationScaleSetting();
        float GetTransitionAnimationScaleSetting();
        float GetWindowAnimationScaleSetting();
        bool IsRemoveAnimationsOn();
        float GetCurrentAnimatorDurationScale();
        void OverrideAnimatorDuration();
        void RevertAnimatorDuration(bool isfinal);
        string GetBaseUrl();
        string GetAssetsPath();
        string GetAbsoluteOnDemandAssetPath(string assetPack);
        string GetAbsoluteOnDemandAssetPath(string assetPack, string relativeAssetPath);
        void RequestAppReview(ContentPage page);
        int FetchOnDemandPack(string pack);
        event EventHandler<AssetPackStatusEventArgs> OnDemandPackStatusNotification;

        //void SaveFileToDownloads(byte[] data, string name);
    }
}
