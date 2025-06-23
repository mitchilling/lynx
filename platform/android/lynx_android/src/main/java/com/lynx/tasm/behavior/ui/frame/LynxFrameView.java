// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

package com.lynx.tasm.behavior.ui.frame;

import android.content.Context;
import android.util.AttributeSet;
import androidx.annotation.RestrictTo;
import com.lynx.tasm.LynxLoadMeta;
import com.lynx.tasm.TemplateBundle;
import com.lynx.tasm.behavior.LynxContext;
import com.lynx.tasm.behavior.ui.UIBody.UIBodyView;

@RestrictTo(RestrictTo.Scope.LIBRARY)
public final class LynxFrameView extends UIBodyView {
  private LynxFrameRender mRender;

  public LynxFrameView(Context context) {
    super(context);
    init(context);
  }

  public LynxFrameView(Context context, AttributeSet attrs) {
    super(context, attrs);
    init(context);
  }

  private void init(Context context) {
    mRender = new LynxFrameRender((LynxContext) context, this);
  }

  void loadBundle(TemplateBundle bundle) {
    LynxLoadMeta.Builder builder = new LynxLoadMeta.Builder();
    builder.setTemplateBundle(bundle);
    mRender.loadTemplate(builder.build());
  }

  void destroy() {
    mRender.destroy();
  }
}
