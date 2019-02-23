#include "../src/database.h"

class DatabaseMock : public Database
{
public:
	// fetch urls of individual addon id
	Plan fetch(const std::string& id) const override;
};
