# ifndef TENANTTAG_H
# define TENANTTAG_H

# include "ns3/tag.h"
namespace ns3 {
  class TenantTag: public Tag {
    public:
      /**
      * \brief Get the type ID.
      * \return the object TypeId
      */
      static TypeId GetTypeId (void);
      TenantTag();
      virtual TypeId GetInstanceTypeId (void) const;
      virtual uint32_t GetSerializedSize (void) const;
      virtual void Serialize (TagBuffer buf) const;
      virtual void Deserialize (TagBuffer buf);
      virtual void Print (std::ostream &os) const;
      void SetTenantId(uint32_t tenant);
      uint32_t GetTenantId() const;

    private:
      uint32_t tenantId;
  };
}

# endif