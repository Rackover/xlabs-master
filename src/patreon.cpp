#include "std_include.hpp"
#include "patreon.hpp"
#include "console.hpp"

#include "utils/http.hpp"

namespace
{
	void patrons_get(patreon::patron_list& patrons, patreon& p)
	{
		std::string url =
			"https://www.patreon.com/api/oauth2/v2/campaigns/3669088/members?include=currently_entitled_tiers,address,user&fields%5Bmember%5D=full_name,is_follower,last_charge_date,last_charge_status,lifetime_support_cents,currently_entitled_amount_cents,patron_status&fields%5Btier%5D=amount_cents,created_at,description,discord_role_ids,edited_at,patron_count,published,published_at,requires_shipping,title,url&fields%5Baddress%5D=addressee,city,line_1,line_2,phone_number,postal_code,state&fields%5Buser%5D=full_name,social_connections,hide_pledges&page%5Bcount%5D=100";

		while (!url.empty())
		{
			auto result = p.execute(url);
			if (!result)
			{
				throw std::runtime_error{"Unable to get data"};
			}

			rapidjson::Document document{};
			document.Parse(result->data(), result->size());

			std::unordered_set<std::string> hidden_users{};
			const auto& included = document["included"];
			for (rapidjson::SizeType i = 0; i < included.Size(); ++i)
			{
				auto& entry = included[i];
				auto& type = entry["type"];
				if (!type.IsString() || type.GetString() != "user"s)
				{
					continue;
				}

				std::string id = entry["id"].GetString();
				auto hide = entry["attributes"]["hide_pledges"].GetBool();
				if (hide)
				{
					hidden_users.emplace(id);
				}
			}

			const auto& data = document["data"];
			for (rapidjson::SizeType i = 0; i < data.Size(); ++i)
			{
				auto& entry = data[i];
				auto& attributes = entry["attributes"];
				auto& name = attributes["full_name"];
				auto& status = attributes["patron_status"];
				if (!status.IsString() || status.GetString() != "active_patron"s)
				{
					continue;
				}

				std::string id = entry["relationships"]["user"]["data"]["id"].GetString();
				if (hidden_users.contains(id))
				{
					continue;
				}

				auto full_name = std::string{name.GetString(), name.GetStringLength()};
				if (full_name.find_first_of(' ') != std::string::npos)
				{
					// Skip real names for now
					continue;
				}

				patrons.emplace_back(std::move(full_name));
			}

			if (!document.HasMember("links"))
			{
				break;
			}

			url = document["links"]["next"].GetString();
		}
	}
}

patreon::patreon(std::string secret)
	: secret_(std::move(secret))
{
}

void patreon::get_patrons(const std::function<void(patron_list)>& callback)
{
	std::thread([this, callback]()
	{
		patron_list patrons{};
		try
		{
			patrons_get(patrons, *this);
		}
		catch (std::exception& e)
		{
			console::error("Fetching patrons failed: %s", e.what());
		}

		callback(std::move(patrons));
	}).detach();
}

std::optional<std::string> patreon::execute(const std::string& url) const
{
	return utils::http::get_data(url, {{"Authorization", "Bearer " + this->secret_}});
}
