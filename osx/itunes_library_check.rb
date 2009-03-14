#!/usr/bin/env ruby
require 'find'
require 'plist'
require 'uri'

itunes_path = File.join ENV['HOME'], 'Music', 'iTunes'
music_path = File.join itunes_path, 'iTunes Music'
lib_path = File.join itunes_path, 'iTunes Music Library.xml'

puts ":: Parsing iTunes Library File..."
itunes = Plist::parse_xml lib_path
library = itunes['Tracks'].values
tracks = []

puts ':: Scanning for Missing Files...'
library.each do |track|
  location = URI.unescape track["Location"].sub("file://localhost", "")
  tracks << location.downcase
  puts "Missing   #{location}" unless File.file? location
end

puts ':: Scanning for orphaned files...'
Find.find music_path do |path|
  next if File.directory? path
  puts "Orphaned  #{path}" unless tracks.delete(path.downcase)
end
