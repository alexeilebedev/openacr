#!/usr/bin/env node
// Copyright (C) 2025-2026 AlgoRND
//
// Check that file contains a valid JSON object

'use strict';

import * as fs from 'fs';

// Get file path from command line argument
const filePath = process.argv[2];

if (!filePath) {
    console.error('Usage: check-json.mjs <path-to-config>');
    process.exit(1);
}

try {
    const data = fs.readFileSync(filePath, 'utf-8');
    JSON.parse(data);
} catch (err) {
    console.error(`Invalid ${filePath}:`, err.message);
    process.exit(1);
}

process.exit(0);
