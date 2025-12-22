// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

package com.lynx.devtoolwrapper;

import static org.junit.Assert.*;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class DevToolLifecycleTest {
  private DevToolLifecycle mLifecycle;

  @Before
  public void setUp() {
    mLifecycle = DevToolLifecycle.getInstance();
    mLifecycle.reset();
  }

  @Test
  public void testInitialState() {
    assertEquals(DevToolState.UNAVAILABLE, mLifecycle.getState());
    assertFalse(mLifecycle.isAttached());
    assertFalse(mLifecycle.isEnabled());
    assertFalse(mLifecycle.isInitialized());
    assertFalse(mLifecycle.isConnected());
  }

  @Test
  public void testOnAttached() {
    mLifecycle.onAttached();
    assertEquals(DevToolState.ATTACHED, mLifecycle.getState());
    assertTrue(mLifecycle.isAttached());
    assertFalse(mLifecycle.isEnabled());
  }

  @Test
  public void testOnEnabled() {
    mLifecycle.onAttached();
    mLifecycle.onEnabled();
    assertEquals(DevToolState.ENABLED, mLifecycle.getState());
    assertTrue(mLifecycle.isEnabled());
    assertFalse(mLifecycle.isInitialized());
  }

  @Test
  public void testOnInitialized() {
    mLifecycle.onAttached();
    mLifecycle.onEnabled();
    mLifecycle.onInitialized();
    assertEquals(DevToolState.INITIALIZED, mLifecycle.getState());
    assertTrue(mLifecycle.isInitialized());
    assertFalse(mLifecycle.isConnected());
  }

  @Test
  public void testOnConnected() {
    mLifecycle.onAttached();
    mLifecycle.onEnabled();
    mLifecycle.onInitialized();
    mLifecycle.onConnected();
    assertEquals(DevToolState.CONNECTED, mLifecycle.getState());
    assertTrue(mLifecycle.isConnected());
  }

  @Test
  public void testOnDisconnected() {
    mLifecycle.onAttached();
    mLifecycle.onEnabled();
    mLifecycle.onInitialized();
    mLifecycle.onConnected();

    mLifecycle.onDisconnected();
    assertEquals(DevToolState.INITIALIZED, mLifecycle.getState());
  }

  @Test
  public void testOnDisabled() {
    mLifecycle.onAttached();
    mLifecycle.onEnabled();
    mLifecycle.onInitialized();

    mLifecycle.onDisabled();
    assertEquals(DevToolState.ATTACHED, mLifecycle.getState());

    // Re-enable to check if we can go back
    mLifecycle.onEnabled();
    mLifecycle.onInitialized();
    mLifecycle.onConnected();
    mLifecycle.onDisabled();
    assertEquals(DevToolState.ATTACHED, mLifecycle.getState());
  }

  @Test
  public void testInvalidTransitions() {
    // Cannot go UNAVAILABLE -> ENABLED directly
    mLifecycle.onEnabled();
    assertEquals(DevToolState.UNAVAILABLE, mLifecycle.getState());

    // Cannot go ATTACHED -> INITIALIZED directly (need ENABLED first)
    mLifecycle.onAttached();
    mLifecycle.onInitialized();
    assertEquals(DevToolState.ATTACHED, mLifecycle.getState());

    // Cannot go ENABLED -> CONNECTED directly (need INITIALIZED first)
    mLifecycle.onEnabled();
    mLifecycle.onConnected();
    assertEquals(DevToolState.ENABLED, mLifecycle.getState());
  }
}
