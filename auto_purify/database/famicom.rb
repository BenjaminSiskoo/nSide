require("D:/RubyLibraries/launcher/markup/document.rb")
famicom_hpp = "string Famicom = R\"(\n"
sleep(1.0)
[
  "Famicom (NTSC)",
  "Nintendo Entertainment System (NTSC)",
  "Nintendo Entertainment System (PAL)"
].each_with_index do |database_name,index|
  database_path = "D:/RubyLibraries/launcher/database/#{database_name}.bml"
  database = File.read(database_path,mode:"rb").force_encoding(
    Encoding::UTF_8
  )
  database[0,1] = "" if database[0,1] == 0xFEFF.chr(database.encoding)
  if index != 0
    database[/database revision=[0-9]{4}-[0-9]{2}-[0-9]{2}\n\n/] = ""
  end
  famicom_hpp << "\n" << database << "\n"
end
famicom_hpp << "\n)\";\n"
puts("Saving famicom.hpp to \"#{__dir__}\"")
File.write("#{__dir__}/famicom.hpp",famicom_hpp,mode:"wb")