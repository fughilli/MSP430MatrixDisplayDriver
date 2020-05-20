#ifndef GAMELIB_CONFIG_H
#define GAMELIB_CONFIG_H

#define ASPECT_RATIO_16_9 0x0001
#define ASPECT_RATIO_4_3 0x0002

#define PLATFORM_ARDUINO 0x0010
#define PLATFORM_TIVAC 0x0020
#define PLATFORM_SIMULATOR 0x0040

/* MODIFY THESE OPTIONS TO MATCH YOUR PLATFORM
 */

/* Aspect ratio
 *
 * This option sets the aspect ratio of the framebuffer,
 * and the resulting screen resolution
 *
 * options:
 *
 * ASPECT_RATIO_16_9    :   16x9    (1080p, 720p (most modern TVs))
 * ASPECT_RATIO_4_3     :   4x3     (480p (older TVs))
 */
#define ASPECT_RATIO ASPECT_RATIO_16_9

/* Platform options:
 *
 * PLATFORM_ARDUINO         :   Arduino console
 * PLATFORM_TIVAC           :   TivaC console
 * (PLATFORM_SIMULATOR |
 *  PLATFORM_TIVAC)         :   Desktop simulator, configured to emulate TivaC
 * (PLATFORM_SIMULATOR |
 *  PLATFORM_ARDUINO)       :   Desktop simulator, configured to emulate Arduino
 */
#define PLATFORM (PLATFORM_ARDINO)

#endif // GAMELIB_CONFIG_H

