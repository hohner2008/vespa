// Copyright Verizon Media. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
package com.yahoo.vespa.config.server;

import com.yahoo.cloud.config.ConfigserverConfig;
import com.yahoo.concurrent.InThreadExecutorService;
import com.yahoo.config.model.NullConfigModelRegistry;
import com.yahoo.config.model.api.ConfigDefinitionRepo;
import com.yahoo.config.provision.Provisioner;
import com.yahoo.config.provision.Zone;
import com.yahoo.container.jdisc.secretstore.SecretStore;
import com.yahoo.vespa.config.server.application.PermanentApplicationPackage;
import com.yahoo.vespa.config.server.application.TenantApplicationsTest;
import com.yahoo.vespa.config.server.filedistribution.FileDistributionFactory;
import com.yahoo.vespa.config.server.modelfactory.ModelFactoryRegistry;
import com.yahoo.vespa.config.server.tenant.MockTenantListener;
import com.yahoo.vespa.config.server.tenant.TenantListener;
import com.yahoo.vespa.flags.FlagSource;
import com.yahoo.vespa.flags.InMemoryFlagSource;
import com.yahoo.vespa.model.VespaModelFactory;

import java.nio.file.Files;
import java.time.Clock;
import java.util.Collections;
import java.util.Optional;
import java.util.concurrent.ExecutorService;

import static com.yahoo.yolean.Exceptions.uncheck;

/**
 * @author Ulf Lilleengen
 */
public class TestComponentRegistry implements GlobalComponentRegistry {

    private final ConfigserverConfig configserverConfig;
    private final ConfigDefinitionRepo defRepo;
    private final ReloadListener reloadListener;
    private final TenantListener tenantListener;
    private final PermanentApplicationPackage permanentApplicationPackage;
    private final ModelFactoryRegistry modelFactoryRegistry;
    private final Optional<Provisioner> hostProvisioner;
    private final Zone zone;
    private final Clock clock;
    private final ConfigServerDB configServerDB;
    private final ExecutorService zkCacheExecutor;
    private final SecretStore secretStore;
    private final FlagSource flagSource;

    private TestComponentRegistry(ModelFactoryRegistry modelFactoryRegistry,
                                  PermanentApplicationPackage permanentApplicationPackage,
                                  ConfigserverConfig configserverConfig,
                                  Optional<Provisioner> hostProvisioner,
                                  ConfigDefinitionRepo defRepo,
                                  ReloadListener reloadListener,
                                  TenantListener tenantListener,
                                  Zone zone,
                                  Clock clock,
                                  SecretStore secretStore,
                                  FlagSource flagSource) {
        this.configserverConfig = configserverConfig;
        this.reloadListener = reloadListener;
        this.tenantListener = tenantListener;
        this.defRepo = defRepo;
        this.permanentApplicationPackage = permanentApplicationPackage;
        this.modelFactoryRegistry = modelFactoryRegistry;
        this.hostProvisioner = hostProvisioner;
        this.zone = zone;
        this.clock = clock;
        this.configServerDB = new ConfigServerDB(configserverConfig);
        this.zkCacheExecutor = new InThreadExecutorService();
        this.secretStore = secretStore;
        this.flagSource = flagSource;
    }

    public static class Builder {
        private ConfigserverConfig configserverConfig = new ConfigserverConfig(
                new ConfigserverConfig.Builder()
                        .configServerDBDir(uncheck(() -> Files.createTempDirectory("serverdb")).toString())
                        .configDefinitionsDir(uncheck(() -> Files.createTempDirectory("configdefinitions")).toString())
                        .sessionLifetime(5));
        private ConfigDefinitionRepo defRepo = new StaticConfigDefinitionRepo();
        private ReloadListener reloadListener = new TenantApplicationsTest.MockReloadListener();
        private final MockTenantListener tenantListener = new MockTenantListener();
        private final Optional<FileDistributionFactory> fileDistributionFactory = Optional.empty();
        private ModelFactoryRegistry modelFactoryRegistry = new ModelFactoryRegistry(Collections.singletonList(new VespaModelFactory(new NullConfigModelRegistry())));
        private Optional<Provisioner> hostProvisioner = Optional.empty();
        private Zone zone = Zone.defaultZone();
        private Clock clock = Clock.systemUTC();
        private FlagSource flagSource = new InMemoryFlagSource();

        public Builder configServerConfig(ConfigserverConfig configserverConfig) {
            this.configserverConfig = configserverConfig;
            return this;
        }

        public Builder modelFactoryRegistry(ModelFactoryRegistry modelFactoryRegistry) {
            this.modelFactoryRegistry = modelFactoryRegistry;
            return this;
        }

        public Builder provisioner(Provisioner provisioner) {
            this.hostProvisioner = Optional.ofNullable(provisioner);
            return this;
        }

        public Builder zone(Zone zone) {
            this.zone = zone;
            return this;
        }

        public Builder clock(Clock clock) {
            this.clock = clock;
            return this;
        }

        public Builder reloadListener(ReloadListener reloadListener) {
            this.reloadListener = reloadListener;
            return this;
        }

        public Builder flagSource(FlagSource flagSource) {
            this.flagSource = flagSource;
            return this;
        }

        public Builder configDefinitionRepo(ConfigDefinitionRepo configDefinitionRepo) {
            this.defRepo = configDefinitionRepo;
            return this;
        }

        public TestComponentRegistry build() {
            final PermanentApplicationPackage permApp = Optional.<PermanentApplicationPackage>empty()
                    .orElse(new PermanentApplicationPackage(configserverConfig));
            SecretStore secretStore = new MockSecretStore();
            return new TestComponentRegistry(modelFactoryRegistry,
                                             permApp,
                                             configserverConfig,
                                             hostProvisioner,
                                             defRepo,
                                             reloadListener,
                                             tenantListener,
                                             zone,
                                             clock,
                                             secretStore,
                                             flagSource);
        }
    }

    @Override
    public ConfigserverConfig getConfigserverConfig() { return configserverConfig; }
    @Override
    public TenantListener getTenantListener() { return tenantListener; }
    @Override
    public ReloadListener getReloadListener() { return reloadListener; }
    @Override
    public ConfigDefinitionRepo getStaticConfigDefinitionRepo() { return defRepo; }
    @Override
    public PermanentApplicationPackage getPermanentApplicationPackage() { return permanentApplicationPackage; }
    @Override
    public ModelFactoryRegistry getModelFactoryRegistry() { return modelFactoryRegistry; }
    @Override
    public Optional<Provisioner> getHostProvisioner() {
        return hostProvisioner;
    }
    @Override
    public Zone getZone() {
        return zone;
    }
    @Override
    public Clock getClock() { return clock;}
    @Override
    public ConfigServerDB getConfigServerDB() { return configServerDB;}

    @Override
    public FlagSource getFlagSource() { return flagSource; }

    @Override
    public ExecutorService getZkCacheExecutor() {
        return zkCacheExecutor;
    }

    @Override
    public SecretStore getSecretStore() {
        return secretStore;
    }

}
