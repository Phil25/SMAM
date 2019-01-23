#include "../src/database.h"

class DatabaseMock : public Database{
public:
	// fetch urls of individual addon id
	std::vector<std::string> fetch(const std::string& id) const override;
};
