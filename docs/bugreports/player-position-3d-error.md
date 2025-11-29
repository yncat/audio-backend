# Bug Report: Player Position 3D Attributes Error

## Commit Hash
`a106dab8887f8fac17b3f03ddd8fc1da2693eb4d`

## Bug Description
When testing menu option 8 ("Test VR Player Position & Sound") in the sample program, the following errors occurred:

1. **First error**: "Failed to set player_sounds group 3D position: Tried to call a command on a 2d sound when the command was meant for 3d sound."
2. **Second error** (after initial fix): "Failed to set 3D attributes on Listener DSP: An invalid parameter was passed to this function."

## Root Cause

The implementation had a fundamental misunderstanding of how Resonance Audio DSP architecture works:

1. **Initial mistake**: Attempted to call `set3DAttributes()` directly on the `player_sounds` channel group, but channel groups don't support 3D positioning directly.

2. **Second mistake**: Attempted to set 3D attributes (parameter 8) on the Resonance Audio **Listener** DSP. However, the Listener DSP automatically receives its position information from FMOD's 3D listener attributes set via `system->set3DListenerAttributes()`, so direct parameter setting is invalid.

3. **Missing architecture**: The `player_sounds` channel group needed a Resonance Audio **Source** DSP attached to it (not a Listener DSP) to position player-emitted sounds in 3D space.

## Fix Implementation

### Files Modified
1. `src/context.h` - Added `vr_player_source_dsp` field and getter/setter methods
2. `src/context.cpp` - Implemented getter/setter and initialized field to nullptr
3. `src/vr.cpp` (lines 165-225) - Attached Resonance Audio Source DSP to player_sounds group
4. `src/vrplayer.cpp` (lines 54-73, 131-150) - Updated to set 3D attributes on Player Source DSP
5. `src/vrobj.cpp` (lines 369-373) - Removed duplicate Source DSP attachment from individual channels

### Solution Details

**Correct Architecture:**
- **Resonance Audio Listener DSP**: Automatically receives position from FMOD's `system->set3DListenerAttributes()`
- **player_sounds group**: Has a Resonance Audio **Source** DSP attached
- **Player position updates**: Updates both FMOD 3D listener and the Player Source DSP's parameter 8

**Implementation:**

1. **Added Player Source DSP storage** (context.h/cpp):
   - Added `FMOD::DSP* vr_player_source_dsp` field
   - Added `GetVrPlayerSourceDsp()` and `SetVrPlayerSourceDsp()` methods

2. **Created and attached Source DSP** (vr.cpp:165-225):
   ```cpp
   // Create Resonance Audio Source DSP for player_sounds group
   FMOD::DSP* playerSourceDsp = nullptr;
   result = system->createDSPByPlugin(source_plugin_handle, &playerSourceDsp);

   // Attach to player_sounds group
   result = playerSoundsGroup->addDSP(FMOD_CHANNELCONTROL_DSP_HEAD, playerSourceDsp);

   // Set distance attenuation (min: 0.5f, max: 200.0f)
   playerSourceDsp->setParameterFloat(2, 0.5f);
   playerSourceDsp->setParameterFloat(3, 200.0f);

   g_context->SetVrPlayerSourceDsp(playerSourceDsp);
   ```

3. **Updated position/rotation functions** (vrplayer.cpp):
   - Changed from `GetVrListenerDsp()` to `GetVrPlayerSourceDsp()`
   - Set 3D attributes on Player Source DSP's parameter 8
   - Also update FMOD's 3D listener via `system->set3DListenerAttributes()`

4. **Removed duplicate DSP attachment** (vrobj.cpp):
   - Removed Source DSP attachment code from `vrOneshotPlayer()`
   - Individual channels now inherit positioning from their parent group's DSP

## Result

- Player position and rotation updates work correctly without errors
- Sounds played via `audio_vrOneshotPlayer()` correctly emit from the player's position
- Resonance Audio spatial processing (including reverb) is properly applied
- The player's "ears" (3D listener) move with the player position

## Testing

Verified with sample program menu option 8:
- Player position updates at various coordinates (origin, left, right, forward)
- Player rotation changes
- Walking simulation with sequential position updates
- All tests completed without errors and sounds were correctly positioned
