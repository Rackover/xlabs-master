curl = {
	source = path.join(dependencies.basePath, "curl"),
}

function curl.import()
	links { "curl" }
	curl.includes()
end

function curl.includes()
	includedirs {
		path.join(curl.source, "include"),
	}

	defines {
		"CURL_STRICTER",
		"CURL_STATICLIB",
	}
end

function curl.project()
	project "curl"
		language "C"

		curl.includes()
		
		includedirs {
			path.join(curl.source, "lib"),
		}

		files {
			path.join(curl.source, "lib/**.c"),
			path.join(curl.source, "lib/**.h"),
		}
		
		defines {
			"BUILDING_LIBCURL",
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, curl)
