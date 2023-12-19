Pod::Spec.new do |spec|

  spec.name         = "libraw"
  spec.version      = "0.0.1"
  spec.summary      = "A short description of PixRaw."

  spec.description  = <<-DESC
                        PixRaw is a library for processing raw image data.
                   DESC
  spec.homepage     = "https://github.com/nenhall/libraw"
  spec.license      = { :type => "MIT", :file => "LICENCE" }
  spec.author             = { "Nenghao Wu" => "nenghao.wu@pixocial.com" }

  spec.ios.deployment_target = "12.0"
  spec.osx.deployment_target = "10.15"
  
  # ――― Source Location ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  spec.source       = { :git => "git@github.com:nenhall/libraw.git", :tag => '#{spec.version}'}

  # ――― Source Code ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  spec.source_files  = "libraw/src/*.{cpp}", "libraw/src/**/*.{h,cpp}", "libraw/libraw/*.{h}", "libraw/internal/*.h"
  spec.exclude_files = "libraw/src/**/*_ph.cpp"
  spec.public_header_files = "libraw/libraw/*.h"
  spec.header_dir    = 'internal'

  # ――― Project Linking ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  spec.library   = "z"

  # ――― Project Settings ――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
#   spec.xcconfig = { "HEADER_SEARCH_PATHS" => "$(SDKROOT)/usr/include/libxml2" }
  spec.pod_target_xcconfig = {
    'HEADER_SEARCH_PATHS' => '"$(SRCROOT)/libraw"' 
  }
  
end
