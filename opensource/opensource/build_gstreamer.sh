#!/bin/bash
# -------------------------------------------------------------------------
#  This file is part of the Vision SDK project.
# Copyright (c) 2025 Huawei Technologies Co.,Ltd.
#
# Vision SDK is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#
#           http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
# -------------------------------------------------------------------------
set -e

echo "start to build gstreamer."
# /home/slave1/.local/bin/meson -v

# Simple log helper functions
info() { echo -e "\033[1;34m[INFO ][Depend  ] $1\033[1;37m" ; }
warn() { echo >&2 -e "\033[1;31m[WARN ][Depend  ] $1\033[1;37m" ; }

work_space="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)"
gstreamerDisabledList=(python libav libnice ugly devtools ges rtsp_server vaapi sharp rs tls qt5 examples nls
  orc gst-examples tests)

baseDisabledPluginList=(adder audioconvert audiomixer audiorate audioresample audiotestsrc compositor encoding gio
 overlaycomposition pbtypes playback rawparse subparse tcp videorate videotestsrc volume
 adder audioconvert audiomixer audiorate audioresample audiotestsrc
 compositor encoding gio overlaycomposition pbtypes playback rawparse subparse tcp videorate
 videotestsrc volume pango opus gl ogg vorbis videoconvertscale drm)

badDisabledPluginList=(accurip adpcmdec adpcmenc aiff videoframe_audiolevel asfmux audiobuffersplit audiofxbad
  audiolatency audiomixmatrix audiovisualizers autoconvert bayer camerabin2 coloreffects debugutils dvbsuboverlay
  dvdspu faceoverlay festival fieldanalysis freeverb frei0r gaudieffects geometrictransform gdp id3tag inter
  interlace ivfparse ivtc jp2kdecimator jpegformat librfb midi mpegdemux mpegtsdemux mpegtsmux mpegpsmux mxf
  netsim onvif pcapparse pnm proxy rawparse removesilence sdp segmentclip siren smooth speed subenc timecode
  videofilters videosignal vmnc y4m directsound wasapi winscreencap winks bluez ipcpipeline opensles
  uvch264 tinyalsa msdk assrender aom voamrwbenc voaacenc bs2b bz2 chromaprint curl dash dc1394
  decklink directfb wayland webp dts resindvd faac faad fbdev flite gsm fluidsynth kms ladspa lv2
  libde265 srt dtls ttml modplug mpeg2enc mplex musepack neon openal opencv openexr openh264 openjpeg
  openmpt openni2 opus rsvg gl vulkan wildmidi smoothstreaming sndfile soundtouch gme dvb sbc
  zbar rtmp spandsp hls x265 webrtcdsp webrtc wpe sctp aes codecalpha
  dvbsubenc nvcodec rist rtmp2 switchbin transcode microdns fdkaac avtp aja v4l2codecs va analyticsoverlay codec2json)

goodDisabledPluginList=(alpha apetag audiofx audioparsers auparse autodetect avi cutter debugutils deinterlace dtmf effectv
  equalizer flv flx goom goom2k1 icydemux id3demux imagefreeze interleave isomp4 law level matroska monoscope multifile
  multipart replaygain shapewipe smpte spectrum udp videobox videocrop videofilter videomixer wavenc wavparse
  y4m directsound waveform oss oss4 aalib cairo flac gtk3 jack jpeg lame libcaca mpg123 pulse dv1394 shout2 soup speex
  taglib twolame vpx wavpack bz2 png dv v4l2-gudev adaptivedemux2)

# Build
fileName="gstreamer"
packageFQDN="gstreamer@1.24.12"
packageName="gstreamer"
addPieArray=(gst-inspect-1.0 gst-typefind-1.0 gst-launch-1.0 gst-stats-1.0 gst-completion-helper gst-ptp-helper
gst-plugin-scanner gst-hotdoc-plugins-scanner gst-play-1.0 gst-device-monitor-1.0 gst-discoverer-1.0 gst-transcoder-1.0
)
normalCompileText="-Wl,-z,relro,-z,now -s"
pieCompileText="-Wl,-z,relro,-z,now -s -pie"

cd "$fileName" || {
  warn "cd to ./opensource/$fileName failed"
  exit 254
}

info "Building dependency $packageFQDN."

if [[ -d "$(pwd)/../tmp/glib/bin" ]]; then
  export PATH=$PATH:$(pwd)/../tmp/glib/bin
else
  exit 254
fi

if [[ -d "./build" ]]; then
  rm -r ./build
fi

if [[ -f "./subprojects/libsoup.wrap" ]];then
  rm "./subprojects/libsoup.wrap"
fi

gstreamerDisabled=$(for plugin in ${gstreamerDisabledList[*]}; do echo -n "-D$plugin=disabled "; done)
badDisabled=$(for plugin in ${badDisabledPluginList[*]}; do echo -n "-Dgst-plugins-bad:$plugin=disabled "; done)
baseDisabled=$(for plugin in ${baseDisabledPluginList[*]}; do echo -n "-Dgst-plugins-base:$plugin=disabled "; done)
goodDisabled=$(for plugin in ${goodDisabledPluginList[*]}; do echo -n "-Dgst-plugins-good:$plugin=disabled "; done)
export LD_LIBRARY_PATH=$(pwd)/../tmp/libffi/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$(pwd)/../tmp/libffi/lib64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$(pwd)/../tmp/libsrtp/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$(pwd)/../tmp/pcre2/lib:$LD_LIBRARY_PATH
sed -i "s/option('gst_debug', type : 'boolean', value : true)/option('gst_debug', type : 'boolean', value : false)/" ${work_space}/gstreamer/subprojects/gstreamer/meson_options.txt
PKG_CONFIG_PATH="$(pwd)/../tmp/glib/lib/pkgconfig:$(pwd)/../tmp/libsrtp/lib/pkgconfig:$(pwd)/../tmp/libffi/lib/pkgconfig:$(pwd)/../tmp/openssl/lib/pkgconfig:$(pwd)/../tmp/pcre2/lib/pkgconfig" \
LDFLAGS="-Wl,-z,relro,-z,now -s" CFLAGS="-D_GLIBCXX_USE_CXX11_ABI -std=gnu99 -fPIE -fstack-protector-all -fPIC -Wall" \
meson --strip \
  --prefix "$(pwd)/../tmp/$packageName" \
  --libdir lib \
  --optimization s \
  -Dbase=enabled \
  -Dbad=enabled \
  -Dgood=enabled \
  -Dgst-plugins-bad:srtp=enabled \
  $badDisabled \
  $baseDisabled \
  $goodDisabled \
  $gstreamerDisabled \
  build || {
  warn "Building $packageFQDN failed during meson build"
  exit 254
}
buildFile=./build/build.ninja
sed -i "s/-Wl,-rpath-link,\/usr\/lib\/aarch64-linux-gnu//g" $buildFile
sed -i "s/-Wl,-rpath-link,\/usr\/lib\/x86_64-linux-gnu//g" $buildFile
if [[ -f "$buildFile" ]]; then
  for name in ${addPieArray[@]}
  do
    searchText="/$name: c_LINKER/="
    lineNum=$(sed -n "$searchText" $buildFile)
    if [[ -z "$lineNum" ]]; then
      echo "gstreamer failed to find $searchText."
    else
      let modLines=$lineNum+1
      sed -i "${modLines}s/${normalCompileText}/${pieCompileText}/" $buildFile
      sed -n "${modLines}p" $buildFile
    fi
  done
fi
ninja -C build || {
  warn "Building $packageFQDN failed during ninja build"
  exit 254
}
ninja -C build install || {
  warn "Building $packageFQDN failed during ninja build install"
  exit 254
}
cd ..
info "Build $packageFQDN done."
