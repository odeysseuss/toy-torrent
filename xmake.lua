add_rules("mode.debug", "mode.release")

target("torrent")
set_kind("binary")
set_languages("c++17")
set_warnings("all", "error")
if is_mode("release") then
    set_optimize("faster")
end
add_includedirs("include")
add_files(
    "src/main.cpp",
    "src/bencode.cpp",
)
