# include "ns3/tag.h"
# include "Tenant-tag.h"

namespace ns3 {
  NS_OBJECT_ENSURE_REGISTERED (TenantTag);
  TypeId TenantTag::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::TenantTag").SetParent<Tag>()
                                       .AddConstructor<TenantTag>();
    return tid;
  }
  
  TenantTag::TenantTag() {
    tenantId = 10233u;
  }

  TypeId TenantTag::GetInstanceTypeId(void) const {
    return GetTypeId();
  }

  uint32_t TenantTag::GetSerializedSize(void) const {
    return sizeof(uint32_t);
  }

  void TenantTag::Serialize(TagBuffer i) const {
    i.WriteU32(tenantId);
  }

  void TenantTag::Deserialize(TagBuffer i) {
    tenantId = i.ReadU32();
  }

  void TenantTag::Print(std::ostream &os) const {
    os << "Tenant ID: " << tenantId;
  }

  void TenantTag::SetTenantId(uint32_t tenant) {
    tenantId = tenant;
  }

  uint32_t TenantTag::GetTenantId() const {
    return tenantId;
  }
}