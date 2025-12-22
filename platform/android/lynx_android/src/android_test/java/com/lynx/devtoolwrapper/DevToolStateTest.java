// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

package com.lynx.devtoolwrapper;

import static org.junit.Assert.*;

import org.junit.Test;

public class DevToolStateTest {
  /**
   * DevToolLifecycle uses Enum.ordinal() to compare states
   * (e.g. isAttached() checks state >= ATTACHED).
   * This test ensures that the order of constants in DevToolState enum
   * remains correct for those comparisons.
   * <p>
   * Expected order:
   * 1. UNAVAILABLE
   * 2. ATTACHED
   * 3. ENABLED
   * 4. INITIALIZED
   * 5. CONNECTED
   */
  @Test
  public void testStateOrdinality() {
    assertTrue("ATTACHED should be after UNAVAILABLE",
        DevToolState.ATTACHED.ordinal() > DevToolState.UNAVAILABLE.ordinal());
    assertTrue("ENABLED should be after ATTACHED",
        DevToolState.ENABLED.ordinal() > DevToolState.ATTACHED.ordinal());
    assertTrue("INITIALIZED should be after ENABLED",
        DevToolState.INITIALIZED.ordinal() > DevToolState.ENABLED.ordinal());
    assertTrue("CONNECTED should be after INITIALIZED",
        DevToolState.CONNECTED.ordinal() > DevToolState.INITIALIZED.ordinal());
  }
}
