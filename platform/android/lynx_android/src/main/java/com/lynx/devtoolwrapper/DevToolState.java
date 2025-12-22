// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

package com.lynx.devtoolwrapper;

/**
 * Represents the lifecycle states of the DevTool.
 *
 * <pre>
 * State Transition Graph:
 *
 * [UNAVAILABLE]
 *      |
 *      | (onAttached)
 *      v
 *   [ATTACHED] <------------------------------------+
 *      |                                            |
 *      | (onEnabled)                                |
 *      v                                            |
 *    [ENABLED]                                      |
 *      |                                            |
 *      | (onInitialized)                            |
 *      v                                            |
 *   [INITIALIZED] <----------+                      | (onDisabled)
 *      |                     |                      |
 *      | (onConnected)       | (onDisconnected)     |
 *      v                     |                      |
 *  [CONNECTED]  -------------+                      |
 *      |                                            |
 *      +--------------------------------------------+
 * </pre>
 */
public enum DevToolState {
  /**
   * DevTool is not integrated into the application.
   */
  UNAVAILABLE,

  /**
   * DevTool is present, but disabled by app policy or user switch.
   */
  ATTACHED,

  /**
   * DevTool is enabled by app policy or user switch, waiting for initialization.
   */
  ENABLED,

  /**
   * DevTool environment is initialized (e.g. all necessary native libraries loaded)
   * and ready to connect.
   */
  INITIALIZED,

  /**
   * Active inspection or debugging session in progress.
   */
  CONNECTED
}
