// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

package com.lynx.tasm.behavior.ui.frame;

import androidx.annotation.RestrictTo;
import com.lynx.tasm.ILynxEngine;
import com.lynx.tasm.ILynxErrorReceiver;
import com.lynx.tasm.LynxError;
import com.lynx.tasm.LynxLoadMeta;
import com.lynx.tasm.LynxUpdateMeta;
import com.lynx.tasm.behavior.ILynxUIRenderer;
import com.lynx.tasm.behavior.LynxContext;

@RestrictTo(RestrictTo.Scope.LIBRARY)
public final class LynxFrameRender implements ILynxEngine, ILynxErrorReceiver {
  private ILynxUIRenderer mLynxUIRender;
  private long mNativePtr = 0;

  LynxFrameRender(LynxContext context, LynxFrameView view) {
    // TODO(zhoupeng.z): init render with context and view
  }

  public void loadTemplate(final LynxLoadMeta metaData) {
    // TODO(zhoupeng.z): load app bundle with mNativePtr
  }

  public void updateMetaData(LynxUpdateMeta meta) {}

  public void updateViewport(int widthMeasureSpec, int heightMeasureSpec) {}

  public void destroy() {
    // TODO(zhoupeng.z): destroy mNativePtr
  }

  public void onErrorOccurred(LynxError error) {}
}
