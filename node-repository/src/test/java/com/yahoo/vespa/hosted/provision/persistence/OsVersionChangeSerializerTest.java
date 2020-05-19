// Copyright Verizon Media. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
package com.yahoo.vespa.hosted.provision.persistence;

import com.yahoo.component.Version;
import com.yahoo.config.provision.NodeType;
import com.yahoo.vespa.hosted.provision.os.OsVersionChange;
import com.yahoo.vespa.hosted.provision.os.OsVersionTarget;
import org.junit.Test;

import java.time.Duration;
import java.time.Instant;
import java.util.Map;
import java.util.Optional;

import static org.junit.Assert.assertEquals;

/**
 * @author mpolden
 */
public class OsVersionChangeSerializerTest {

    @Test
    public void serialization() {
        var change = new OsVersionChange(Map.of(
                NodeType.host, new OsVersionTarget(NodeType.host, Version.fromString("1.2.3"), Optional.of(Duration.ofHours(1)), Optional.of(Instant.ofEpochMilli(123))),
                NodeType.proxyhost, new OsVersionTarget(NodeType.proxyhost, Version.fromString("4.5.6"), Optional.empty(), Optional.empty()),
                NodeType.confighost, new OsVersionTarget(NodeType.confighost, Version.fromString("7.8.9"), Optional.of(Duration.ZERO), Optional.of(Instant.ofEpochMilli(456)))
        ));
        var serialized = OsVersionChangeSerializer.fromJson(OsVersionChangeSerializer.toJson(change));
        assertEquals(serialized, change);
    }

}
